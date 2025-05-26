#define BLYNK_TEMPLATE_ID "TMPL6wnqoEG5W"
#define BLYNK_TEMPLATE_NAME "FeedMe2"
#include <WiFi.h>
#include <Blynk.h>

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG
#define APP_DEBUG

#include "DHT.h"
#include "BlynkEdgent.h"

#define DHTPIN 19
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;
bool isconnected = false;

float h = 0;
float t = 0;

void setup()
{
  Serial.begin(9600);
  delay(100);
  dht.begin();
  BlynkEdgent.begin();

  timer.setInterval(200L, checkBlynkStatus);
  timer.setInterval(200L, sendSensor);
}

void loop() {
  BlynkEdgent.run();
  timer.run();
}
void checkBlynkStatus() {
  isconnected = Blynk.connected();
  if (isconnected == true) {
    //digitalWrite(wifiled, HIGH);
    Serial.println("Blynk Connected");
  }
  else {
    //digitalWrite(wifiled, LOW);
    Serial.println("Blynk Not Connected");
  }
}

void sendSensor()
{
  delay(2000);
  h = dht.readHumidity();
  t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.println("");
  Serial.print(F("Humidity Room: "));
  Serial.print(h);
  Serial.println(F("% "));
  Serial.print(F("Temperature Room: "));
  Serial.print(t);
  Serial.print(F("°C  "));

  
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  delay(1000);
}
