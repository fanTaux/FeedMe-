#define BLYNK_TEMPLATE_ID "TMPL6wnqoEG5W"
#define BLYNK_TEMPLATE_NAME "FeedMe2"
#include <WiFi.h>
#include <Blynk.h>

#define BLYNK_FIRMWARE_VERSION          "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

#include "DHT.h"
#include "BlynkEdgent.h"

#define DHTPIN 19
#define DHTTYPE DHT11
#define POWER_PIN 17
#define SIGNAL_PIN 36
#define WATER_SENSOR_THRESHOLD 100 

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;
bool isconnected = false;

float h = 0;
float t = 0;

int value = 0;
int water_percentage = 0;

void setup()
{
  Serial.begin(9600);
  delay(100);
  dht.begin();
  BlynkEdgent.begin();

  timer.setInterval(200L, checkBlynkStatus);
  timer.setInterval(2000L, sendSensor); 
  analogSetAttenuation(ADC_11db);
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, LOW);
}

void loop() {
  //BlynkEdgent.run(); 
  timer.run();
}

void checkBlynkStatus() {
  isconnected = Blynk.connected();
  if (isconnected == true) {
    //Serial.println("Blynk Connected"); 
  }
  else {
    //Serial.println("Blynk Not Connected"); 
  }
}

void sendSensor()
{
  
  h = dht.readHumidity();
  t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  digitalWrite(POWER_PIN, HIGH);
  delay(10); 
  value = analogRead(SIGNAL_PIN);
  digitalWrite(POWER_PIN, LOW);

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    // Blynk.virtualWrite(V0, 0); 
    // Blynk.virtualWrite(V1, 0);
    return;
  }

  if (value < WATER_SENSOR_THRESHOLD) {
    water_percentage = 0;
  } else {
    water_percentage = map(value, WATER_SENSOR_THRESHOLD, 1400, 0, 100);
  }

  if (water_percentage < 0) {
    water_percentage = 0;
  } else if (water_percentage > 100) {
    water_percentage = 100;
  }
  
  Serial.println("");
  Serial.print(F("Humidity Room: "));
  Serial.print(h);
  Serial.println(F("% "));
  Serial.print(F("Temperature Room: "));
  Serial.print(t);
  Serial.print(F("°C  "));
  Serial.print("The water sensor raw value: ");
  Serial.print(value);
  Serial.print(", Water Level: ");
  Serial.print(water_percentage);
  Serial.println("%");

  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, water_percentage); 
}
