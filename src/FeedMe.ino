#define BLYNK_TEMPLATE_ID "TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "AUTH_TOKEN"

#include <ESP32Servo.h>
#include <WiFi.h>
#include <WiFiClient.h> 
#include <BlynkSimpleEsp32.h>
#include "DHT.h" 

#define DHTPIN 19
#define DHTTYPE DHT11
#define POWER_PIN 17
#define SIGNAL_PIN 36
#define WATER_SENSOR_THRESHOLD 100
#define PUMP_PIN 14
DHT dht(DHTPIN, DHTTYPE); 
Servo myservo; 
int currentServoPosition; 
int button = 2; 
int manual = 3; 
BlynkTimer timer; 

char auth[] = BLYNK_AUTH_TOKEN; 
char ssid[] = "SSID";
char pass[] = "PASSWORD";

BLYNK_WRITE(V5)
{
  int data = param.asInt();
  // For continuous rotation servo:
  // data = 90 -> stop
  // data < 90 -> rotate one direction (smaller value, faster)
  // data > 90 -> rotate opposite direction (larger value, faster)
  myservo.write(data);
  Blynk.virtualWrite(V5, data); 
}

BLYNK_WRITE(V4) {
  int pumpState = param.asInt();
  digitalWrite(PUMP_PIN, pumpState);
}

void setup() {
  Serial.begin(9600); 
  delay(100);

  dht.begin(); 

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  myservo.attach(26);
  myservo.write(90);

  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);
  Blynk.syncVirtual(V0); 
  Blynk.syncVirtual(V1); 
  Blynk.syncVirtual(V2); 
  Blynk.syncVirtual(V4); 
  Blynk.syncVirtual(V3); 
  Blynk.syncVirtual(V5); 

  timer.setInterval(2000L, sendSensorData); 

  analogSetAttenuation(ADC_11db); 
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, LOW); 

  pinMode(button, INPUT_PULLUP);
  pinMode(manual, INPUT_PULLUP);
}

void loop() {
  Blynk.run();
  timer.run();   
}

void sendSensorData() {
  if (!Blynk.connected()) {
    Serial.println("Blynk not connected, skipping sensor data transmission.");
    return;
  }

  float h = dht.readHumidity();
  float t = dht.readTemperature(); 

  digitalWrite(POWER_PIN, HIGH); 
  delay(10); 
  int value = analogRead(SIGNAL_PIN); 
  digitalWrite(POWER_PIN, LOW); 

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  int water_percentage = 0;
  if (value < WATER_SENSOR_THRESHOLD) {
    water_percentage = 0;
  } else {
    water_percentage = map(value, WATER_SENSOR_THRESHOLD, 4095, 0, 100);
  }

  if (water_percentage < 0) {
    water_percentage = 0;
  } else if (water_percentage > 100) {
    water_percentage = 100;
  }

  int read_manual = digitalRead(manual);
  int read_button = digitalRead(button);

  currentServoPosition = myservo.read(); 
  Blynk.virtualWrite (V3, String(currentServoPosition)); 

  if (read_button == LOW && read_manual == HIGH) {
    Serial.println("Button pressed, dispensing food...");
    myservo.write(0); 
    delay(500); 
    myservo.write(90);
    Serial.println("Servo stopped after dispensing.");
  }
  else if (read_button == HIGH && read_manual == HIGH) { 
    myservo.write(90); 
    Serial.println("Servo is idle (stopped).");
  }
  else if (read_manual == LOW) {
    Serial.println("Manual mode active, controlling servo from Blynk V5.");
  }
  
  Serial.println("\n--- Sensor Data ---");
  Serial.print("Room Humidity: ");
  Serial.print(h);
  Serial.println("%");
  Serial.print("Room Temperature: ");
  Serial.print(t);
  Serial.println("°C");
  Serial.print("Raw water sensor value: ");
  Serial.print(value);
  Serial.print(", Water Level: ");
  Serial.print(water_percentage);
  Serial.println("%");
  Serial.println("-------------------");

  Blynk.virtualWrite(V0, t);           
  Blynk.virtualWrite(V1, h);           
  Blynk.virtualWrite(V2, water_percentage); 
}

