# FeedMe! - Automation of Animal Feed & Water Feeding for Modern Chicken Farm based on IoT

<p align="center">
  <img src="https://github.com/user-attachments/assets/fed6c4f2-d08b-470d-a71d-a3c96022bd6b" alt="FeedMe Project Banner" width="600">
</p>

## Project Domain
This project presents an Internet of Things (IoT) solution that automates key aspects of livestock care. It features an automated system for dispensing feed and water via actuators and integrates real-time environmental monitoring to ensure optimal temperature conditions.

## Table Of Contents
- [Background](#background)
- [Prerequisites](#prerequisites)
- [System Diagrams](#system-diagrams)
- [Demo and Evaluation](#demo-and-evaluation)
- [Conclusion](#conclusion)
- [Team](#team)

---

## Background


### Problem Statements
1. Feeding and watering chickens is still performed manually, requiring the farmer's constant physical presence.
2. Farm management is inefficient in terms of time and energy due to repetitive daily tasks.
3. The temperature inside the chicken coop is unpredictable and not consistently monitored, which can negatively affect chicken health and growth.
   
### Goals
1. Automate the feeding and watering process with remote control and scheduling via a mobile app.
2. Improve time and energy efficiency for the farmer, allowing focus on other productive activities.
3. Enable real-time monitoring of the coop's temperature to maintain an ideal environment.

### Solution Statements
1. Build an IoT-based automatic feeding and watering system using actuators like servo motors and waterpump.
2. Develop a mobile application to serve as a control dashboard for scheduling, manual commands, and monitoring.
3. Integrate a temperature sensor (e.g., DHT11) to provide real-time environmental data on the mobile app.
4. Use an ESP32 microcontroller to control the entire system, including sensors, actuators, and Wi-Fi connectivity.

---

## Prerequisites

### Hardware Specifications

- **Microcontroller:** ESP32 DEV KIT v4
- **Sensors:** Temperature Sensor (DHT 11) & Water Level Sensor
- **Actuators:** Servo motor (SG90) & Waterpump (5V)
- **Switching:** Relay (5V)
- **Connectivity:** WiFi
- **Power Supply:** 5V Powerbank

### ESP32 Datasheet
<img src="assets/esp32v4pinout.png" alt="ESP32 Pinout" width="500">

### Blynk User Interface
<img src="assets/blynk.jpg" alt="Blynk User Interface" width="450">

## System Diagrams

1.  **Block Diagram**

    ![Block Diagram](assets/blockdiagram.jpg)

2.  **Sequence Diagram**

    ![Sequence Diagram](assets/sequencediagram.jpg)

3.  **Schematic**

    ![Schematic Diagram](assets/Schematic.jpg)


## Demo and Evaluation
### Demo Link : https://youtu.be/uSfvWD3atIk?si=YR0Dsns08SiuMoGj

- **Setup:** Assemble all components based on the provided schematic diagram. Once the hardware is connected, upload the source code to the ESP32 microcontroller.
- **Demo:** The demonstration will showcase the system's core functions, including real-time temperature and water level monitoring. It will also feature the remote activation of the servo motor to dispense feed and the water pump to provide drinking water.
- **Evaluation:** To assess performance and durability, the system will be tested in a real-world environment, such as a chicken coop. This evaluation will measure the device's robustness and effectiveness under actual operational conditions.

## Conclusion
This IoT project can save cost, time, and effort for chicken farmers. Its automatic features can provide benefits for chicken farmers. With this product we can also bring a better life with future technology. As long as we can use this product well, we can develop chicken farming with better techniques and provide better efficiency for other activities.

## Team
1. Nathanael Erens Susanto  (235150300111044)
2. Faris Arinanta           (235150300111045)
3. Achmad Fiky Akbar        (235150301111043)
4. Nabilla Suci Amanda      (235150301111044)
5. Ahmad Maulana Maky       (235150307111038)
