# Joystick Controller Using ESP-NOW

## Table of Contents
1. [Description](#description)
2. [Features](#features)
3. [Hardware Requirements](#hardware-requirements)
4. [Software Requirements](#software-requirements)
5. [Setup](#setup)
6. [Configuration](#configuration)
7. [Usage](#usage)
8. [Circuit Diagram](#circuit-diagram)
9. [Troubleshooting](#troubleshooting)

## Description
This project implements a joystick controller using ESP-NOW, a protocol developed by Espressif for low-power, peer-to-peer communication between ESP8266 devices. The joystick can be used to control various devices wirelessly, making it ideal for remote control applications.

## Features
- **Wireless Communication:** Utilizes ESP-NOW for low-latency communication between ESP8266 devices.
- **Easy Setup:** Simple installation and configuration process for quick deployment.
- **Low Power Consumption:** Designed for low-power operations, suitable for battery-powered applications.
- **Versatile Applications:** Can be used for various remote control applications such as drones and robots.

## Hardware Requirements
- 2 x ESP8266 boards (e.g., NodeMCU)
- 1 or 2 Joystick modules (You can use the Y-axis in one joystick and the X-axis in another joystick)
- 2 x DC motors (for the receiver/car)
- L298N Motor Driver (or similar)
- Jumper wires
- Power supply for the motors

## Software Requirements
- [Arduino IDE](https://www.arduino.cc/en/software)
- ESP8266 Board Support in Arduino IDE
- ESP8266WiFi library
- espnow library

## Setup
1. Install the required libraries in your Arduino IDE.
2. Upload the `transmitter.ino` code to one ESP8266 board (the controller).
3. Upload the `receiver.ino` code to another ESP8266 board (the robot).
4. Connect the joystick module to the transmitter board according to the pin definitions in the code.
5. Connect the motors and motor driver to the receiver board as per the pin definitions.

## Configuration
1. In `transmitter.ino`, replace the `receiverMAC` array with the MAC address of your receiver ESP8266.
2. Adjust the `SERIAL_PORT` boolean in both files if you want to enable/disable serial debugging.
3. In `receiver.ino`, adjust the following parameters to suit your application:
   - `const int DEAD_ZONE = 10;` // Set the dead zone to avoid unnecessary movements when the joystick is close to the center.
   - `const int numSamples = 100;` // Set the number of samples to average for finding the center position of the joystick.

## Usage
1. Power on both ESP8266 boards.
2. Observe the wireless communication in action as you move the joystick.

## Circuit Diagram
![Example Transmitter Circuit](circuit/transmitter.png)

## Troubleshooting
- **Issue:** Devices do not communicate.
  - **Solution:** Ensure both ESP8266 boards are powered and within range. Check the code for correct Wi-Fi credentials and ESP-NOW setup.
  
- **Issue:** Joystick is not responding.
  - **Solution:** Verify the connections between the joystick module and the ESP8266. Check for any loose wires.

- **Issue:** Compilation errors in Arduino IDE.
  - **Solution:** Ensure you have installed the ESP8266 board support and the required libraries.

- **Issue:** Joystick movements are erratic or unresponsive near the center position.
  - **Solution:** Adjust the `DEAD_ZONE` value in `receiver.ino`. A larger dead zone can help eliminate noise and prevent unintended movements. Experiment with different values until you find one that works well for your joystick.

## Credits
Created by NIHAL T P  
GitHub: [nihaltp](https://github.com/nihaltp)