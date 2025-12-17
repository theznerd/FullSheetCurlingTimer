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

### ESP-NOW

The ESP-NOW protocol is used for low-latency, connectionless communication
between the secondary timer devices and the main timer device. This allows for
quick transmission of laser trip events without the overhead of traditional
Wi-Fi connections and higher reliability than BLE advertising alone.

#### ESP-NOW Provisioning

Provisioning is designed to require **no MAC address entry** and no external
app. All devices are configured only via DIP switches and then automatically
discover and associate at boot.

- **DIP switch identity**
  - `sheetId` (3 bits): identifies the sheet (supports up to 8 sheets).
  - `locationId` (2 bits): identifies placement/role.
    - `00` = **Main Timer** (ESP-NOW receiver/aggregator)
    - `01`–`11` = **Secondary Sensors** (ESP-NOW transmitters)
- **Boot-time discovery (same firmware image)**
  - All devices initialize ESP-NOW on a fixed, predefined Wi-Fi channel.
  - **Main Timer (`locationId=00`)** periodically broadcasts a short
    `MAIN_BEACON` containing at minimum `{sheetId, mainMac, epoch}`.
  - **Secondary sensor (`locationId!=00`)** listens for a matching
    `MAIN_BEACON` (same `sheetId`), then caches `mainMac` and sends a
    `SENSOR_JOIN` message containing `{sheetId, locationId, sensorMac, epoch}`.
  - The Main Timer records `(locationId -> sensorMac)` and may optionally reply
    with a unicast `JOIN_ACK` so the sensor can confirm it is paired to the
    correct Main Timer.
- **Conflict handling**
  - If two devices present the same `(sheetId, locationId)`, the Main Timer
    treats this as a configuration error (DIP collision) and should reject or
    flag the duplicate sender.
- **Persistence**
  - Learned MACs will be cached in RAM for the session but will not be stored in
    non-volatile storage to speed reconnection after reboot for simplicity and
    ease of replacement should devices be swapped out.

#### Message format (minimal, fixed-size)

ESP-NOW payloads are kept small, fixed-size, and binary to minimize airtime and
parsing overhead. All messages begin with a common header so receivers can
quickly filter by `sheetId` and `msgType`.

- **Common header (all messages)**
  - `uint8_t  version` (start at `1`)
  - `uint8_t  msgType` (enum below)
  - `uint8_t  sheetId` (0-7)
  - `uint8_t  locationId` (0-3; `0` = main, `1-3` = sensors)
  - `uint16_t epoch` (per-boot *session id* used to prevent `eventId` collisions
    after reboot; generated randomly at boot by default)
  - `uint16_t crc16` (CRC of the message excluding the crc field)

- **Message types**
  - `MAIN_BEACON` (broadcast)
    - `uint8_t  mainMac[6]`
    - `uint8_t  channel` (ESP-NOW channel in use)
    - `uint32_t mainTimeUs32` (Main Timer local timebase, e.g.,
      `esp_timer_get_time()` truncated)
  - `SENSOR_JOIN` (broadcast or unicast to `mainMac`)
    - `uint8_t sensorMac[6]`
  - `JOIN_ACK` (unicast)
    - `uint8_t mainMac[6]`
    - `uint8_t accepted` (0/1)
  - `TRIP_EVENT` (unicast)
    - `uint32_t eventId` (monotonic per sensor per epoch)
    - `uint8_t  eventCode` (beam broken/restored, etc.)
    - `uint32_t sensorTimeUs32` (**timestamp captured at detection time** on the
      sender)

Notes:

- `epoch` exists so that if a device reboots and its `eventId` counter restarts,
  the Main Timer can still distinguish "new events after reboot" from delayed or
  duplicated packets from the prior session.
- CRC is optional but strongly recommended to avoid false triggers from corrupt
  frames in a noisy RF environment.
- If the Main Timer derives "event time" from *packet arrival*, then any
  retry/burst spacing (and RF contention) directly becomes timestamp error. To
  avoid that, **timestamp at the sensor** and transmit the capture time in
  `TRIP_EVENT`. The Main Timer can then map `sensorTimeUs32` into its own
  timebase using the most recent `MAIN_BEACON` time correlation.

#### Timing and retry policy (8-sheet environment)

The goal is fast join and high reliability without excessive airtime when many
sheets are active.

- **ESP-NOW channel**
  - Use a fixed channel for all devices (e.g., channel `1`, `6`, or `11`).
  - Transmit the channel in `MAIN_BEACON` for diagnostics.

- **Join window (boot-time discovery)**
  - On boot, all devices run a discovery window of **10 seconds**.
  - The Main Timer continues to beacon at a low rate after the window so late
    sensors can still join.

- **Main beacon interval**
  - During discovery window: **250 ms** interval (fast association).
  - After discovery window: **2 s** interval (low background airtime).

- **Sensor join attempts**
  - After receiving a matching `MAIN_BEACON`, a sensor sends `SENSOR_JOIN` and
    waits for `JOIN_ACK`.
  - Retry `SENSOR_JOIN` up to **5 times** with randomized backoff:
    - base delay **150 ms** + random jitter **0-150 ms**.
  - If no ack arrives after retries, fall back to listening for beacons and
    repeat.

- **Trip event delivery (recommended)**
  - Use **unicast** to `mainMac` once learned.
  - On each laser trip, send a short burst to mitigate collisions/interference:
    - **3 transmissions** total per event
    - spacing: **25 ms** + random jitter **0-20 ms**
  - Main Timer dedupes by `(sheetId, locationId, epoch, eventId)`.
  - **Important**: the burst spacing should not affect the *recorded* time when
    `sensorTimeUs32` is used as the source-of-truth timestamp. The spacing only
    affects how quickly the Main Timer *learns* about the event, not when the
    event is considered to have occurred.

- **Timebase alignment (recommended for accurate splits)**
  - Each `MAIN_BEACON` includes `mainTimeUs32`.
  - Each sensor records `(mainTimeUs32, sensorLocalTimeUs32_at_beacon_rx)` for
    the latest beacon(s) and maintains an estimated offset:
    - `offsetUs ~= mainTimeUs32 - sensorTimeUs32_at_beacon_rx`
  - When sending `TRIP_EVENT`, the Main Timer can estimate:
    - `eventMainTimeUs ~= sensorTimeUs32 + offsetUs`
  - This keeps “registered time” decoupled from RF retries and burst jitter,
    with residual error dominated by beacon receive latency and clock drift
    (typically far smaller than 25–90 ms).

### Bluetooth Low Energy (BLE) - GATT Server

The main timer device acts as a BLE GATT server, advertising a custom "Timer
Service" for viewer devices (browsers) to connect to and receive timing updates
via notifications.
