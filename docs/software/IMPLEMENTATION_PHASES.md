# Implementation Phases - Firmware and Bring-up

Last update: 2026-02-11

## Phase 0 - Bring-up board

Goal:
- validate buses, power stability, and pin map.

Deliverables:
- I2C scan
- GPS UART parse test
- LoRa ping/ack test
- SD write test

Acceptance:
- all mandatory interfaces validated on real hardware.

## Phase 1 - Flight core

Goal:
- enable tracker + basic flight instrument logic.

Deliverables:
- QMI8658 integration
- BMP581 integration
- GNSS parser integration
- LoRa payload v1 periodic uplink

Acceptance:
- 1h stable run with no unexpected reset.

## Phase 2 - Balloon meteo mode

Goal:
- provide meteorological telemetry profile.

Deliverables:
- BME688 integration
- low power profile
- extended LoRa packet with meteo fields

Acceptance:
- stable temperature/humidity/pressure telemetry in field test.

## Phase 3 - Robustness and logging

Goal:
- improve reliability for long missions.

Deliverables:
- SD logging service
- health fault counters
- watchdog handling

Acceptance:
- >3h mission test with valid logs and telemetry.
