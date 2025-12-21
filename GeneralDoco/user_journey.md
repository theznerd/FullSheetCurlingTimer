# User Journeys and Use Cases

This document holds considerations for how users will interact with the curling
timer system, including setup, operation, and maintenance. This document should
be used to guide design decisions in firmware, enclosure, and overall system
architecture.

## Preconfiguration

### Device Identification

The devices have a 5-position DIP switch for configuration of identity. The
first 3 positions set the `groupId` (0-7) to identify which group the device
belongs to (could be used as a sheet number). The last 2 positions set the
`deviceId` (0-3) to identify the role/placement of the device within the group:

- `00` = **Main Timer** (ESP-NOW receiver/aggregator and BLE peripheral)
- `01` = **Sensor 1 - Nearest Hog Line** (ESP-NOW transmitter)
- `10` = **Sensor 2 - Opposing Hog Line** (ESP-NOW transmitter)
- `11` = **Sensor 3 - Opposing Back Line** (ESP-NOW transmitter)

A group of four devices (one main timer and three sensors) should be configured
with the same `groupId` and unique `deviceId`s to form a complete set for one
sheet.

## Setup Journeys

### Charging

Charging is handled via TP4056 modules with USB-C input. Users should be able to
easily connect a USB-C cable to each device for charging. Two LED indicators on
the module show charging status (red = charging, green = charged [NOTE: NEEDS
CONFIRMATION OF COLORS]). The TP4056 modules include overcharge protection, so
users can leave devices connected without risk of damage.

### Power On/Off

Each device includes a rocker switch for power control. Users should toggle the
switch to the "on" position to power on the device. There is no LED indicator
for power status, but the device will emit a short beep from the piezo buzzer
upon power on to indicate it is active.

All devices in a group should be powered on within 30 seconds of each other to
ensure proper ESP-NOW pairing and synchronization. The order of powering on the
devices should be designed to not matter. If the (non-main sensors) are not
powered on within 30 seconds of the main timer, it may take longer for pairing
to complete.

### Placement and Alignment

Users should place the devices at their designated locations down the sheet.
Room should be left behind each device for a curler to come around without
tripping the laser beam when returning from delivering a stone. On the opposite
side of the sheet from the device, the reflecting hardware (retroreflective tape
applied to a 3d printed mount) should be placed to reflect the laser beam back
to the phototransistor in the device.

- **Main Timer (Device ID: 00)**: Positioned on the back line
- **Sensor 1 (Device ID: 01)**: Positioned on the near hog line
- **Sensor 2 (Device ID: 10)**: Positioned on the far hog line
- **Sensor 3 (Device ID: 11)**: Positioned on the far back line

### Connection to Viewer

Users should connect to the main timer device using the provided native app,
This app will guide users through scanning for nearby timer devices, selecting
the correct one based on advertised name, and connecting to it. Once connected,
the native app will display the current timer state and provide controls for
validating the orientation/alignment of the laser sensors and retroreflectors.

## Operation Journeys

### Recording a Delivery

The user starts a timer by delivering a stone. When the stone crosses the laser
on either back lines, the main timer device records the start time and begins
timing (there are two timer threads to account for both directions, should an
accidental trip occur either because of throwing the stone through the house or
another curler crossing the beam). Upon crossing the back line laser, a
debounce period of one second is initiated to prevent multiple trips from being
recorded in quick succession and the initial trip of the back line laser sends
an arming signal to the hog line sensors. Based on which hog line sensor is
tripped next, the main timer records the split time from the appropriate back
line sensor (for back line to hog line split). When the stone crosses the
opposite hog line laser, the hog-to-hog split time is recorded. Each sensor
crossing event inclues a short beep from the piezo buzzer to provide audio
feedback to the curler that the trip was registered. Additionally, the sensors
record the laser break and restore times to provide a general speed of the
stone. This should allow the curler to get a rough idea of whether they are
positive or negative on their throw speed and by how much.

### Viewing Results

The native app connected to the main timer device will display timing results in
real-time as the stone crosses each laser sensor. The UI will show the split
times for back line to hog line and hog line to hog line, as well as the
approximate stone speed based on laser break and restore times. The UI will
also provide visual indicators for each sensor trip event and any errors or
issues encountered during timing.

### Resetting the Timer

In the event of a false start, accidental trips, or if the stone does not cross
the opposing hog line, the user can reset the timer via the native app. If the
user prefers to wait, a new delivery can be started by simply crossing the back
line laser again after the debounce period has elapsed. Partial times will still
be available in the native app for review, but can be cleared from the native
app if desired.

## Maintenance Journeys

### Battery Charge Monitoring

The native app can provide a battery charge status indicator based on voltage
readings from an adc pin on the ESP32 connected to the battery voltage divider.
The voltage divider solution is not highly accurate, but should provide a rough
indication of battery health. Users can monitor this status in the native app
and be alerted when the battery charge is low and needs recharging.

### Battery Replacement

The devices use a single 18650 Li-ion battery for power. When the battery
reaches end-of-life or fails to hold a charge, users should be able to easily
replace the battery. The enclosure is designed to come apart with a few screws,
allowing access to the battery holder. Users can remove the old battery and
insert a new 18650 battery, ensuring correct polarity. The enclosure can then
be reassembled and the device powered on for use.

### Firmware Updates

At least initially, firmware updates will require connecting the ESP32 dev board
to a computer via USB and using the Arduino IDE or ESP32 flashing tools to
upload new firmware. As these devices are hobbyist-focused, users with some
technical knowledge should be able to perform firmware updates as needed. In
the future, over-the-air (OTA) updates could be considered to simplify this
process.
