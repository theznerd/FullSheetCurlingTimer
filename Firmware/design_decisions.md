# Firmware Design Decisions

This document outlines the design decisions made for the firmware of the trip
sensor for the Full Sheet Curling Timer project.

## Firmware Architecture

- **Single Firmware Image**: A single firmware image is used for both the main
  timer device (BLE Peripheral) and the secondary timer devices (ESP-NOW
  Senders), with behavior determined at runtime based on DIP switch settings.
- **Development Board Target**: The firmware is designed to run on ESP32
  development boards to keep the project hobbyist friendly and avoid the need
  for custom PCB designs for the microcontroller.

## Wireless Communication

- **ESP-NOW Protocol**: The ESP-NOW protocol is used for low-latency,
  connectionless communication between the secondary timer devices and the main
  timer device. This allows for quick transmission of laser trip events without
  the overhead of traditional Wi-Fi connections and higher reliability than BLE
  advertising alone.
- **GATT Server**: The main timer device acts as a BLE GATT server, advertising
  a custom "Timer Service" for viewer devices (browsers) to connect to and
  receive timing updates via notifications.

