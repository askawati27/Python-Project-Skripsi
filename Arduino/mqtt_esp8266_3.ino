#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>

// MPU6050 specific
#define MPU6050_FS_SEL0 3
#define MPU6050_FS_SEL1 4
#define MPU6050_AFS_SEL0 3
#define MPU6050_AFS_SEL1 4

// Combined definitions for the FS_SEL values.eg.  ñ250 degrees/second
#define MPU6050_FS_SEL_250  (0)
#define MPU6050_FS_SEL_500  (bit(MPU6050_FS_SEL0))
#define MPU6050_FS_SEL_1000 (bit(MPU6050_FS_SEL1))
#define MPU6050_FS_SEL_2000 (bit(MPU6050_FS_SEL1) | bit(MPU6050_FS_SEL0))

// Combined definitions for the AFS_SEL values
#define MPU6050_AFS_SEL_2G  (0)
#define MPU6050_AFS_SEL_4G  (bit(MPU6050_AFS_SEL0))
#define MPU6050_AFS_SEL_8G  (bit(MPU6050_AFS_SEL1))
#define MPU6050_AFS_SEL_16G (bit(MPU6050_AFS_SEL1)|bit(MPU6050_AFS_SEL0))

// See page 12 & 13 of MPU-6050 datasheet for sensitivities config and corresponding output      
#define GYRO_FULL_SCALE_RANGE  MPU6050_FS_SEL_250    
#define GYRO_SCALE_FACTOR      131     // LSB / (degs per seconds)
#define ACC_FULL_SCALE_RANGE   MPU6050_AFS_SEL_4G    
#define ACC_SCALE_FACTOR       8192    // LSB / g
// Update these with values suitable for your network.

const char* ssid = "AndroidAPJ";
const char* password = "naonaihh";
const char* mqtt_server = "192.168.43.146";//192.168.43.146

#define PERIOD  4000    // loop period in micros
static int MPU_ADDR = 0x68; //AD0 is LOW

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int id_device=2;
String message;

static float GYRO_RAW_TO_DEGS = 1.0 / (1000000.0 / PERIOD) / GYRO_SCALE_FACTOR;

int16_t accX, accY, accZ;
int16_t gyroX, gyroY, gyroZ;
int16_t gyroX_calibration, gyroY_calibration, gyroZ_calibration;

void setup_mpu() {
  Wire.begin();
  Wire.setClock(400000L);

  //By default the MPU-6050 sleeps. So we have to wake it up.
  Wire.beginTransmission(MPU_ADDR);     
  Wire.write(0x6B);   //We want to write to the PWR_MGMT_1 register (6B hex)
  Wire.write(0x00);   //Set the register bits as 00000000 to activate the gyro
  Wire.endTransmission();
         
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1B);   //We want to write to the GYRO_CONFIG register (1B hex)
  Wire.write(GYRO_FULL_SCALE_RANGE);                    
  Wire.endTransmission();

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1C);   //We want to write to the ACCEL_CONFIG register (1A hex)
  Wire.write(ACC_FULL_SCALE_RANGE);                    
  Wire.endTransmission();
  //Set some filtering to improve the raw data.
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1A);    //We want to write to the CONFIG register (1A hex)
  Wire.write(0x03);    //Set the register bits as 00000011 (Set Digital Low Pass Filter to ~43Hz)
  Wire.endTransmission();
  calibrateGyro();
}
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40

void getAcceleration(int16_t* x, int16_t* y, int16_t* z) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(ACCEL_XOUT_H);
  Wire.endTransmission();  
  Wire.requestFrom(MPU_ADDR, 6);
  *x = constr((((int16_t)Wire.read()) << 8) | Wire.read(), -ACC_SCALE_FACTOR, ACC_SCALE_FACTOR);
  *y = constr((((int16_t)Wire.read()) << 8) | Wire.read(), -ACC_SCALE_FACTOR, ACC_SCALE_FACTOR);
  *z = constr((((int16_t)Wire.read()) << 8) | Wire.read(), -ACC_SCALE_FACTOR, ACC_SCALE_FACTOR);
}

#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48

void getRotation(int16_t* x, int16_t* y, int16_t* z) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(GYRO_XOUT_H);
  Wire.endTransmission();
  Wire.requestFrom(MPU_ADDR, 6);  
  *x = ((((int16_t)Wire.read()) << 8) | Wire.read()) - gyroX_calibration;
  *y = ((((int16_t)Wire.read()) << 8) | Wire.read()) - gyroY_calibration;
  *z = ((((int16_t)Wire.read()) << 8) | Wire.read()) - gyroZ_calibration;
}
//process data gyro
void calibrateGyro() {
  int32_t x, y, z;
  
  for(int i=0; i<500; i++){
    getRotation(&gyroX, &gyroY, &gyroZ);
    x += gyroX;
    y += gyroY;
    z += gyroZ;

    delayMicroseconds(PERIOD); // simulate the main program loop time ??
  }
  gyroX_calibration = x / 500;
  gyroY_calibration = y / 500;
  gyroZ_calibration = z / 500;
}

// on ESP32 Arduino constrain doesn't work
int16_t constr(int16_t value, int16_t mini, int16_t maxi) {
  if (value < mini)
    return mini;
  else if (value > maxi)
    return maxi;
  return value;
}
float constrf(float value, float mini, float maxi) {
  if (value < mini)
    return mini;
  else if(value > maxi)
    return maxi;
  return value;
}

uint32_t loop_timer;
float roll, pitch, rollAcc, pitchAcc;
float speeed;


void setup() {
 // pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  setup_mpu();
  loop_timer = micros() + PERIOD;
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if ((char)payload[0] == '1') {
//    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level//
  } else {
//    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
//      client.publish("outTopic");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop() {
  static unsigned long mtime=0;
  byte nData;
 getAcceleration(&accX, &accY, &accZ);
  rollAcc = asin((float)accX / ACC_SCALE_FACTOR) * RAD_TO_DEG;
  pitchAcc = asin((float)accY / ACC_SCALE_FACTOR) * RAD_TO_DEG;

  getRotation(&gyroX, &gyroY, &gyroZ);

  // roll vs pitch depends on how the MPU is installed in the robot
  roll -= gyroY * GYRO_RAW_TO_DEGS;
  pitch += gyroX * GYRO_RAW_TO_DEGS;

   roll = roll * 0.999 + rollAcc * 0.001;
  pitch = pitch * 0.999 + pitchAcc * 0.001;

if(1){
  Serial.print(" AcX = "); Serial.print(accX);
  Serial.print(" AcY = "); Serial.print(accY);
  Serial.print(" AcZ = "); Serial.print(accZ);
  Serial.print(" GyX = "); Serial.print(gyroX);
  Serial.print(" GyY = "); Serial.print(gyroY);
  Serial.print(" GyZ = "); Serial.print(gyroZ);
//  Serial.print(" | Roll = "); Serial.print(roll);
//  Serial.print(" | Pitch = "); Serial.println(pitch);
} 
message =(String(accX)+"," +String(accY)+","+String(accZ)+","+String(gyroX)+","+String(gyroY)+","+String(gyroZ));
  while(loop_timer > micros());
  loop_timer += PERIOD;
 

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
 if ( millis() - 20 >= mtime ) {
    char bufD[message.length()+1];
    message.toCharArray(bufD,message.length()+1);
    Serial.println(bufD);
    client.publish("Topic",bufD);
    mtime = millis();
     }
  }
