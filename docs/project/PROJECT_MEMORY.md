# Project Memory - HAT v1 for Waveshare ESP32-S3

Last update: 2026-02-14
Status: LOCKED v1 baseline

## Mission

Create a bottom-mounted HAT PCB for Waveshare ESP32-S3 Touch AMOLED 1.64
with multi-use capability:
- flight basic instrumentation
- weather balloon telemetry
- GPS tracker/beacon

## Locked decisions

1. Mechanical stack:
- sandwich under base board with 4x M2 standoffs

2. Radio:
- LoRa at 915 MHz

3. GPS:
- external module on 1x6 2.54 mm header

4. v1 functional scope:
- telemetry + flight basic + balloon meteo

5. Power:
- use Waveshare BAT/3V3 rails

6. Manufacturing package:
- Gerber + BOM as mandatory output

## Locked v1 modules

1. LoRa SX1262/CC68
2. QMI8658 IMU
3. BMP581 barometer
4. BME688 meteo sensor
5. External GPS connector
6. I2C/UART expansion header

## Base board touch decision

- Class: `LOCKED` (for current base board)
- Controller: FT3168 on shared I2C (GPIO47/GPIO48), address `0x38`
- Firmware mode: polling (INT/RST not required)

## Deferred (v2)

1. Mesh routing
2. Repeater-specific behavior
3. Non-GNSS satellite receive scenarios
4. Optional sensor expansion depending on board area

Note:
- Firmware now contains a `CANDIDATE` MeshLoRa protocol baseline for software validation.
- Product lock is unchanged: mesh/repeater remains deferred for formal v1 scope.

## Risks to control

1. Pin map mismatch between docs and firmware.
2. RF and power noise affecting sensor stability.
3. Oversized scope delaying first manufacturable revision.

## Working rule

Any hardware-impacting change must update:
- `src/config/pins.h`
- `docs/hardware/PINMAP.md`
- `docs/process/AGENT_STATUS.md`
