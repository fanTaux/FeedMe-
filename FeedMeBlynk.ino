#define BLYNK_TEMPLATE_ID "TMPL6wnqoEG5W" // Gunakan template ID dari FeedMe2
#define BLYNK_TEMPLATE_NAME "FeedMe2"     // Gunakan template Name dari FeedMe2

// Sertakan library yang dibutuhkan
#include <ESP32Servo.h>
#include <WiFi.h>
#include <BlynkEdgent.h> // Penting untuk Blynk Edgent (provisioning Wi-Fi)
#include "DHT.h"         // Untuk sensor DHT


// Definisi pin dan tipe sensor DHT
#define DHTPIN 19
#define DHTTYPE DHT11

// Definisi pin dan threshold sensor air
#define POWER_PIN 17
#define SIGNAL_PIN 36
#define WATER_SENSOR_THRESHOLD 100 // Sesuaikan threshold ini jika diperlukan

Servo myservo; // Objek Servo global
int posisiSekarang; // Variabel untuk menyimpan posisi servo saat ini
int button = 2;
int manual = 3;

BlynkTimer timer; // Objek BlynkTimer global

DHT dht(DHTPIN, DHTTYPE); // Objek sensor DHT

BLYNK_WRITE(V5)
{
  int data = param.asInt(); 
  myservo.write(data);
  Blynk.virtualWrite(V5, data);
} 



void setup() {
  Serial.begin(9600); // Mulai komunikasi serial
  delay(100);

  dht.begin(); // Inisialisasi sensor DHT

  // Inisialisasi Blynk Edgent. Ini akan menangani provisioning Wi-Fi.
  BlynkEdgent.begin(); 

  // Pasang servo ke GPIO 26
  myservo.attach(26); 

  // Sinkronkan semua pin virtual yang relevan dengan server Blynk.
  // Ini memastikan widget di aplikasi menunjukkan status awal yang benar.
  Blynk.syncVirtual(V0); // Untuk suhu
  Blynk.syncVirtual(V1); // Untuk kelembaban
  Blynk.syncVirtual(V2); // Untuk level air

  // Atur timer untuk mengirim data sensor secara berkala
  timer.setInterval(2000L, sendSensorData); // Kirim data sensor setiap 2 detik.

  // Konfigurasi pin daya sensor air
  analogSetAttenuation(ADC_11db); // Atur atenuasi ADC untuk rentang yang lebih luas
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, LOW); // Jaga daya mati saat tidak membaca
}

void loop() {
  BlynkEdgent.run(); // Ini harus terus berjalan untuk fungsionalitas Blynk
  timer.run();       // Ini menjalankan timer Blynk
}

// Fungsi untuk membaca dan mengirim data sensor ke Blynk
void sendSensorData() {
  // Pastikan Blynk terhubung sebelum menulis ke pin virtual
  if (!Blynk.connected()) {
    Serial.println("Blynk tidak terhubung, melewati pengiriman data sensor.");
    return;
  }

  // Baca data sensor DHT
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // atau dht.readTemperature(true) untuk Fahrenheit

  // Baca data sensor air
  digitalWrite(POWER_PIN, HIGH); // Nyalakan daya ke sensor
  delay(10); // Penundaan singkat agar sensor stabil
  int value = analogRead(SIGNAL_PIN); // Baca nilai analog
  digitalWrite(POWER_PIN, LOW); // Matikan daya untuk menghemat energi

  // Tangani kesalahan pembacaan DHT
  if (isnan(h) || isnan(t)) {
    Serial.println("Gagal membaca dari sensor DHT!");
    return;
  }

  // Hitung persentase air
  int water_percentage = 0;
  if (value < WATER_SENSOR_THRESHOLD) {
    water_percentage = 0;
  } else {
    // Petakan nilai sensor mentah ke persentase (sesuaikan 1400 jika nilai maks sensor Anda berbeda)
    water_percentage = map(value, WATER_SENSOR_THRESHOLD, 1400, 0, 100); 
  }

  // Batasi persentase air ke rentang 0-100
  if (water_percentage < 0) {
    water_percentage = 0;
  } else if (water_percentage > 100) {
    water_percentage = 100;
  }

  int read_manual = digitalRead(manual);
  int read_button = digitalRead(button);
  posisiSekarang = myservo.read();
  Blynk.virtualWrite (V3,String(posisiSekarang));
  if(read_button == 1 && read_manual == 0) {
    myservo.write(90);
  } else if (read_button == 0 && read_manual == 0) {
    myservo.write(0);
  }

  // Cetak data sensor ke Serial Monitor
  Serial.println("\n--- Data Sensor ---");
  Serial.print("Kelembaban Ruangan: ");
  Serial.print(h);
  Serial.println("%");
  Serial.print("Suhu Ruangan: ");
  Serial.print(t);
  Serial.println("°C");
  Serial.print("Nilai mentah sensor air: ");
  Serial.print(value);
  Serial.print(", Level Air: ");
  Serial.print(water_percentage);
  Serial.println("%");
  Serial.println("-------------------");

  // Kirim data sensor ke pin virtual Blynk
  Blynk.virtualWrite(V0, t);             // Kirim suhu ke V0
  Blynk.virtualWrite(V1, h);             // Kirim kelembaban ke V1
  Blynk.virtualWrite(V2, water_percentage); // Kirim level air ke V2
  
}
