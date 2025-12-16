# 🥌🥌🥌 Full Sheet Curling Timer 🥌🥌🥌

The purpose of this repository is to explore an inexpensive, ESP32-based, laser
trip timer for full sheet timing (4 lasers, 2 at each back line and 2 at each
hog line). Access to the times recorded by the timer is provided via a simple
web interface that connects to an ESP32 using the Web Bluetooth API. One ESP32
acts as a BLE Peripheral that hosts a custom GATT service for the timer, and all
four ESP32 devices will act as event detectors (laser trip detection) which
communicate the event data via ESP-NOW protocol to be picked up by the BLE
peripheral.

![Top-down view of sheet with emitters and reflectors](sheet-diagram.png)

## License

This project is licensed under the Business Source License 1.1 (BUSL).
You may use it freely for noncommercial purposes.
For commercial use, please contact [nathan@z-nerd.com](mailto:nathan@z-nerd.com)
to obtain a commercial license.

## Goals

- **💸 Low-cost hardware**: inexpensive components (e.g., ESP32, laser modules)
  to keep single sheet cost under $150 (less when building for multiple sheets).
- **✅ Easy fabrication**: 3D-printable enclosure and PCBs for hobbyist skill
  level assembly (or schematics for breadboard assembly).
- **⏱️ Accurate timing**: millisecond-level accuracy for curling timing.
- **⚙️ Easy Setup and Use**: non-technical friendly setup and operation with
  guidance for setup and calibration provided in the web UI.
- **👀 Full sheet support**: support dual split timing (back line to hog line,
  and hog line to hog line) and bi-directional throws without moving hardware.
- **👨‍👩‍👧‍👦 Multi-viewer support**: allow multiple phones/tablets to connect and view
  timing data simultaneously.
- **🌐 Web-based UI**: simple web interface accessible from any modern browser
  with Web Bluetooth support (e.g., Chrome, Edge) - no cloud services, no
  complicated Wi-Fi setup for devices.

## BLE Architecture (Multi-viewer)

### Roles and data flow

- **Main Timer and Laser Sensor - ESP32 = BLE Peripheral (GATT Server)**
  - Advertises a custom "Timer Service" so clients can discover it.
  - Has an assigned sheet ID (via dip switches) for identification.
  - Has an assigned placement ID (via dip switches) for sheet position.
  - Receives laser trip events from self and secondary timer devices (ESP-NOW).
  - Hosts characteristics for state + events.
  - Pushes timing updates using **notifications**.

- **Secondary Timer Devices - ESP32s = ESP-NOW Sender**
  - Has an assigned sheet ID (via dip switches) for identification.
  - Has an assigned placement ID (via dip switches) for sheet position.
  - Communicates laser trip events via ESP-NOW protocol.

- **Viewer (browser/web UI) = BLE Central (GATT Client)**
  - Scans for timer devices (filtered by name/service UUID).
  - Connects to **one** timer device at a time.
  - Provides diagnostics and setup UI.
  - Reads current timer state upon connection.
  - Subscribes to notifications to receive live updates.

This pattern supports multiple viewers concurrently: each viewer connects and
enables notifications independently.

### Advertising / device identification

To make it easy to pick the right timer when multiple devices are nearby, each
device should include clear identity in advertising:

- **Device name**: e.g., `CurlTimer Sheet 3`
- **Advertised Service UUID**: `Timer Service` (custom UUID)
- **Optional service/manufacturer data**: e.g., `sheetId`, `deviceId`,
  `firmwareVersion`

### GATT Service Model (proposed)

A single custom service which contains a small set of characteristics:

- **Timer Events** (`Notify`)
  - Streams *incremental* one-way event updates (laser trips, split times,
    etc.).
  - Viewers subscribe once; ESP32 pushes updates when they occur.
  - Events should include a monotonically increasing sequence number so clients
    can detect gaps.

- **Timer State** (`Read` + `Notify`)
  - A bounded *snapshot* of the current timer state (idle/running/finished),
    direction, current/last time, etc.
  - Includes **recent history** (e.g., up to **N** most recent recorded times)
    so late-joining/reconnected viewers can render immediately.
  - Keep this payload compact and capped (fixed max N) so it remains fast to
    read/notify.

- **Control** (`Write`, optional)
  - Commands from a viewer to the ESP32 (e.g., `reset`, `delete last time`,
    `set direction`).
  - If reverse control is not needed, this characteristic can be omitted.

### Notes for future consideration

- It is possible to infer the direction of travel from the sequence of laser
  trip events, so that a stone passing the opposing back line does not cause a
  new timing event. However, this may be error-prone if stones are being thrown
  closely together in timing in opposite directions. Explicit direction setting
  from the viewer may be more reliable but requires user interaction which may
  be inconvenient or undesirable during practice.
- It may be useful to calculate the rock speed at each trip point and include
  that in the event data sent to viewers.
