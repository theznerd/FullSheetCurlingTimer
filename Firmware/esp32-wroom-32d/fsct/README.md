# Full Sheet Curling Timer Firmware

Firmware for the Full Sheet Curling Timer project.

## Overview

This project implements embedded firmware for timing curling sheet events using
the an ESP32 microcontroller on a development board.

## Tested Hardware

- **ESP32-WROOM-32D Development Board**: Generic ESP32 dev board with the
    ESP32-WROOM-32D module.

## Build System

- **CMake**: Cross-platform build configuration
- **ESP-IDF**: Espressif IoT Development Framework

## Project Structure

```text
fsct/
├── main/              # Application source code
│   ├── adc/           # ADC driver implementation
│   ├── gatt/          # Bluetooth GATT service definitions
│   └── gpio/          # GPIO configuration
├── include/           # Header files
├── .devcontainer/     # Docker development environment
├── .vscode/           # VS Code configuration
└── build/             # Build artifacts (generated - excluded from repo)
```

## Getting Started

Use whatever you want - I don't care. I'm using VSCode with the ESP-IDF
extension, and I've installed v5.5.1 of the ESP-IDF toolkit. I don't use any of
the python scripts directly - I point an click in the ESP-IDF explorer to build,
flash, and monitor.

### Prerequisites

- ESP-IDF toolkit (v5.5.1 tested)

### Development Setup

Use the included Dev Container:

```bash
docker-compose up -d
```

Or configure manually with ESP-IDF.

### Building

```bash
idf.py build
```

### Flashing

```bash
idf.py flash
```

## Configuration

Edit `sdkconfig` or use the menuconfig tool:

```bash
idf.py menuconfig
```

## Version

See `version.txt` for current firmware version.
