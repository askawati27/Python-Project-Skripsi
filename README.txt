==================DATA======================================================
Perangkat sensor MPU605O dan mikrokontroler ESP32.
Perangkat sensor diletakkan pada pinggang pengguna saat percobaan, percobaan menggunakan 30 subjek dengan aktivitas seperti :
- aktivitas berjalan
- aktivitas berdiri
- aktivitas duduk
- aktivitas turun tangga
- aktivitas naik tangga

data yang dihasilkan sensor berupa banyak data, waktu, akselerometer dengan sumbu x, sumbu y dan sumbu z. 
Data pada folder data sensor terdiri dari device/banyaknya data, waktu, accx, accy, accz, dan akselerasi.

=> Data Training
Data training diperoleh berdasarkan hasil ekstraksi ciri (Ekstraksi ciri.py) pada setiap aktivitas. 
Data pada folder Data Training terdiri dari data min,max,mean,stdev,median,variance, dan label.

- Data_Berjalan_1 (label=1)
- Data_Berdiri_2 (label=2)
- Data_NaikTangga_3 (label=3)
- Data_TurunTangga_4 (label=4)
- Data_Duduk_5 (label=5)
- Data_Allactivity (Kumpulan seluruh data aktivitas beserta label), menjadi masukkan pada tahap klasifikasi menggunakan file knn.py


=> Data Pengujian
Data pengujian terdiri dari 10 subjek dengan 5 aktivitas yang sama. 
Data pada folder Data Pengujian terdiri dari device/banyaknya data, waktu, accx,accy, dan accz.


=================PROGRAM===============================================
=> Program Arduino
File mqtt_esp8266_3 merupakan program untuk perangkat sensor dan esp32. Dapat mengirimkan data accelerometer(accx, accy, dan accz) , gyroscope(accx, accy, dan accz), pitch, dan roll. Tugas akhir ini hanya menggambil data sensor akselerometer yaitu accx, accy, dan accz.

=>Program Pengolahan Data :
Menggunakan python 2.7
Package :PyWavelet, joblib, matplotlib, seaborn, sklearn, padas, numpy.

Langkah-langkah menjalankan program :
1. file Ekstraksi ciri.py
- pilih folder Data Sensor yang akan di ekstraksi ciri. contoh: ("Data Sensor/Data_Berjalan/")
- Program akan membaca data kolom "Akselerasi" pada semua file.
- Ubah label pada setiap aktivitas 
Data_Berjalan = label 1
Data_Berdiri = label 2
Data_NaikTangga = label 3
Data_TurunTangga = label 4
Data_Duduk = label 5

- Hasil ekstraksi ciri disimpan dengan format .csv contoh("Data Training/Data_Berjalan_1.csv")

Catatan :
Run semua file pada folder data sensor sehingga mendapatkan ciri dan label data setiap aktiitas.

2. file knn.py
- Pilih file pada folder Data Training.Contoh:('Data Training/Data_Allactivity.csv')
- Pada train_test_ratio dapat ditentukan pembagian data training dan testing. Contoh :0.8 (0.8 data training dan 0.2 data testing)
Tugas akhir ini menggunakan pembagian 0.6, 0.7 dan 0.8.
- pada dict_classifier, ubah n_neighbors yang merupakan parameter k . Contoh n_neighbors=3. Ubah metric atau metode pengukuran jarak yang digunakan. Contoh: (Metric='euclidean')
- Run file knn.py untuk mendapatkan nilai akurasi, sensitifitas dan spesifisitas pada setiap metode pengukuran jarak untuk mendapatkan metode pengukuran jarak terbaik.

3. file Testing.py
-Pilih folder Data Testing yang digunakan untuk melakukan prediksi aktivitas. Contoh : ("Data Pengujian/Data Turun Tangga/Subjek_10.csv")
- Run file Testing.py 
- Hasil running menampilkan Hasil prediksi dan waktu running program.





