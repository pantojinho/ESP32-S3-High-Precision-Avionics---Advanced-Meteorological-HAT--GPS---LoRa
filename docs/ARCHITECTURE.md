# Architecture Plan

This document describes the software plan for a non-blocking ESP32-S3 firmware.
It is focused on stability, low latency UI, and clear module boundaries.

## Core Principles
- Never block in the UI loop
- Use dedicated tasks for sensor IO
- Share data via small structs with mutex
- Degrade gracefully when a sensor fails

## Tasks and Rates
1. SensorCore Task (high priority)
- IMU + magnetometer + barometer
- 50-100 Hz update
- Outputs: pitch, roll, yaw, altitude, vertical speed

2. Meteo Task (medium priority)
- BME688 + LTR-390
- 1-5 Hz update
- Outputs: temperature, humidity, pressure, UV, IAQ

3. GNSS Task (medium/low priority)
- UART NMEA parse
- 1-10 Hz update
- Outputs: lat, lon, GPS altitude, ground speed

4. LoRa Task (low priority)
- Packetize and send
- 1-5 Hz update

5. UI Task (medium priority)
- 25-30 FPS render
- Partial redraw to save CPU

6. Black Box Task (low priority)
- Write sensor snapshots to SD
- 1-5 Hz update (configurable)
- Use buffered writes to avoid stalling other tasks

## Data Flow
- Each task updates its own state struct
- UI reads from the shared SystemState
- LoRa reads from SystemState and sends summarized data
- Black Box reads from SystemState and appends to SD log

## UI Menus
1. Aviation Classic
- Artificial horizon
- Altimeter
- Airspeed (from GPS or estimated)
- Vertical speed (vario)
- Heading (from mag)

2. Meteo Station
- Temperature, humidity, pressure
- UV index and light
- IAQ or VOC index (BME688)

3. Telemetry
- LoRa status and RSSI
- GNSS fix quality
- Battery status

## Error Handling
- Watchdog enabled
- I2C timeouts
- If sensor fails, keep UI running and show status

## Memory Strategy
- Internal SRAM for real-time state
- PSRAM only for large buffers (LVGL, history)
- SD logging uses a ring buffer in SRAM; flush in batches

## Build Path
- Start with basic UI and dummy values
- Add IMU and barometer
- Add meteo sensors
- Add GNSS and LoRa
- Add SD Black Box logging (CSV or binary)
- Optimize UI and memory
