import paho.mqtt.client as mqtt
import csv
import sys
import pandas as pd
import time
max=250
data = []
# Define event callbacks
def on_connect(client, userdata, flags, rc):
    print("rc: " + str(rc))

def on_data_full():
    if len(data) == max:       #banyak sample
        with open('Data Pengujian/Data3.csv', 'wb') as csvfile:
            filewriter = csv.writer(csvfile, delimiter=',', quotechar=' ', quoting=csv.QUOTE_MINIMAL)
            # filewriter.writerow(['adl'])
            for i in data:
                filewriter.writerow([i])
        sys.exit()
def on_message(client, obj, msg):
    print(str(msg.payload))
    on_data_full()
    data.append(msg.payload)

def on_publish(client, obj, mid):
    print("mid: " + str(mid))

def on_subscribe(client, obj, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))

def on_log(client, obj, level, string):
    print(string)

mqttc = mqtt.Client()
# Assign event callbacks
mqttc.on_message = on_message
mqttc.on_connect = on_connect

# Parse CLOUDMQTT_URL (or fallback to localhost)
#url_str = os.environ.get('CLOUDMQTT_URL', 'mqtt://localhost:1883')
#url = urlparse.urlparse(url_str)
topic = 'Topic'

# Connect
mqttc.connect("broker.hivemq.com", 1883  )

# Start subscribe, with QoS level 0
mqttc.subscribe(topic, 0)

# Publish a message
#mqttc.publish(topic, i)

# Continue the network loop, exit when an error occurs
mqttc.loop_forever()



