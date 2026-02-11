# Power Budget - HAT v1

Last update: 2026-02-11
Power source: Waveshare BAT/3V3 rails

## Estimated current budget

| Block | Typical (mA) | Peak (mA) | Notes |
| --- | --- | --- | --- |
| ESP32-S3 base board | 80 | 320 | Depends on Wi-Fi/BLE and display load |
| SX1262/CC68 LoRa | 45 | 120 | Peak on TX power bursts |
| BMP581 | 1 | 2 | Low-power barometer |
| QMI8658 | 2 | 4 | Depends on ODR |
| BME688 | 3 | 15 | Gas heater spikes |
| GPS module (external) | 25 | 45 | Depends on model and fix state |
| Misc pullups/LEDs | 3 | 8 | Expansion dependent |
| Total system | 159 | 514 | Target battery and thermal sizing |

## Power modes

1. Idle bench mode:
- LoRa off
- GNSS reduced update rate
- Display reduced refresh

2. Telemetry mode:
- LoRa periodic TX
- GNSS active
- Sensors in nominal ODR

3. Balloon low power mode:
- Lower update rates for meteo and UI
- Burst telemetry interval
- Focus on long runtime

## Design recommendations

1. Decouple each major IC with local 100 nF + bulk capacitor.
2. Keep LoRa power routing short and low impedance.
3. Place RF and sensor returns with clean ground references.
4. Record actual current in bring-up log to calibrate this table.
