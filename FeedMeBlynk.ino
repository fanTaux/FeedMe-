#define BLYNK_TEMPLATE_ID "TMPL6wnqoEG5W"
#define BLYNK_TEMPLATE_NAME "FeedMe2"
#define BLYNK_AUTH_TOKEN "ReEB7EzwlpGF86zWDu6WmU0L1ap81RgI"

// Include necessary libraries
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WiFiClient.h> // Corrected from wiFiClient.h
#include <BlynkSimpleEsp32.h>
#include "DHT.h" // For DHT sensor


// Define DHT sensor pin and type
#define DHTPIN 19
#define DHTTYPE DHT11

// Define water sensor pin and threshold
#define POWER_PIN 17
#define SIGNAL_PIN 36
#define WATER_SENSOR_THRESHOLD 100 // Adjust this threshold if needed

#define PUMP_PIN 14
Servo myservo; // Global Servo object
int currentServoPosition; // Variable to store the current servo position
int button = 2; // Pin for the control button
int manual = 3; // Pin for manual mode (e.g., a switch)

BlynkTimer timer; // Global BlynkTimer object

DHT dht(DHTPIN, DHTTYPE); // DHT sensor object

// --- Wi-Fi Credentials ---
// <--- IMPORTANT: Replace with your actual Wi-Fi SSID and Password
char auth[] = BLYNK_AUTH_TOKEN; // Use the defined BLYNK_AUTH_TOKEN
char ssid[] = "faris";
char pass[] = "sidoarjo";
// --- End Wi-Fi Credentials ---

// Handler for virtual pin V5 from Blynk
BLYNK_WRITE(V5)
{
  int data = param.asInt();
  // For continuous rotation servo:
  // data = 90 -> stop
  // data < 90 -> rotate one direction (smaller value, faster)
  // data > 90 -> rotate opposite direction (larger value, faster)
  myservo.write(data);
  Blynk.virtualWrite(V5, data); // Send value back to Blynk app
}

// Handler for virtual pin V4 from Blynk (moved to global scope)
BLYNK_WRITE(V4) {
  int pumpState = param.asInt();
  digitalWrite(PUMP_PIN, pumpState);
}

void setup() {
  Serial.begin(9600); // Start serial communication
  delay(100);

  dht.begin(); // Initialize DHT sensor

  // Initialize Blynk using explicit Wi-Fi credentials (replaces BlynkEdgent.begin())
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  // Attach servo to GPIO 26
  myservo.attach(26);
  myservo.write(90); // Ensure servo stops at startup (for continuous rotation servo)

  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);
  // Synchronize all relevant virtual pins with the Blynk server.
  // This ensures widgets in the app show the correct initial status.
  Blynk.syncVirtual(V0); // For temperature
  Blynk.syncVirtual(V1); // For humidity
  Blynk.syncVirtual(V2); // For water level
  Blynk.syncVirtual(V4); // For pump control
  Blynk.syncVirtual(V3); // For servo position (if needed for debugging)
  Blynk.syncVirtual(V5); // For servo control from Blynk

  // Set timer to send sensor data periodically
  timer.setInterval(2000L, sendSensorData); // Send sensor data every 2 seconds.

  // Configure water sensor power pin
  analogSetAttenuation(ADC_11db); // Set ADC attenuation for a wider range
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, LOW); // Keep power off when not reading

  // Configure button and manual pins as INPUT_PULLUP if using physical buttons
  pinMode(button, INPUT_PULLUP);
  pinMode(manual, INPUT_PULLUP);
}

void loop() {
  Blynk.run(); // This must keep running for Blynk functionality (replaces BlynkEdgent.run())
  timer.run();     // This runs the Blynk timer
}

// Function to read and send sensor data to Blynk
void sendSensorData() {
  // Ensure Blynk is connected before writing to virtual pins
  if (!Blynk.connected()) {
    Serial.println("Blynk not connected, skipping sensor data transmission.");
    return;
  }

  // Read DHT sensor data
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  // Read water sensor data
  digitalWrite(POWER_PIN, HIGH); // Turn on power to the sensor
  delay(10); // Short delay for sensor to stabilize
  int value = analogRead(SIGNAL_PIN); // Read analog value
  digitalWrite(POWER_PIN, LOW); // Turn off power to save energy

  // Handle DHT reading errors
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Calculate water percentage
  int water_percentage = 0;
  if (value < WATER_SENSOR_THRESHOLD) {
    water_percentage = 0;
  } else {
    // Map raw sensor value to percentage (adjust 4095 if your sensor's max value is different)
    // ESP32 12-bit ADC has a range of 0-4095
    water_percentage = map(value, WATER_SENSOR_THRESHOLD, 4095, 0, 100);
  }

  // Limit water percentage to 0-100 range
  if (water_percentage < 0) {
    water_percentage = 0;
  } else if (water_percentage > 100) {
    water_percentage = 100;
  }

  // Read button and manual switch status
  // Using INPUT_PULLUP, LOW means button is pressed
  int read_manual = digitalRead(manual);
  int read_button = digitalRead(button);

  currentServoPosition = myservo.read(); // Read the last value sent to the servo
  Blynk.virtualWrite (V3, String(currentServoPosition)); // Send to Blynk for debugging

  // Servo control logic for continuous rotation servo
  // If button is pressed (LOW) and manual mode is not active (HIGH)
  if (read_button == LOW && read_manual == HIGH) {
    Serial.println("Button pressed, dispensing food...");
    myservo.write(0); // Rotate full speed in one direction (e.g., to dispense food)
    delay(500); // Rotate for 500 milliseconds (adjust this duration as needed)
    myservo.write(90); // Stop the servo after dispensing
    Serial.println("Servo stopped after dispensing.");
  }
  // If no button is pressed and manual mode is not active
  else if (read_button == HIGH && read_manual == HIGH) { // Button not pressed and manual not active
    myservo.write(90); // Ensure servo is stopped
    Serial.println("Servo is idle (stopped).");
  }
  // If manual mode is active (read_manual == LOW)
  else if (read_manual == LOW) {
    // Let V5 control handle the servo when manual mode is active
    // No need for myservo.write() commands here, as V5 already handles it.
    Serial.println("Manual mode active, controlling servo from Blynk V5.");
  }

  // Print sensor data to Serial Monitor
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

  // Send sensor data to Blynk virtual pins
  Blynk.virtualWrite(V0, t);           // Send temperature to V0
  Blynk.virtualWrite(V1, h);           // Send humidity to V1
  Blynk.virtualWrite(V2, water_percentage); // Send water level to V2
}
