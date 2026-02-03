# StratosVanish S3 - Avionics + Meteo (ESP32-S3)

This repo contains a starter skeleton for a stable, non-blocking firmware on ESP32-S3.
It targets a multi-menu UI (classic aviation instruments + mini meteo station)
using Arduino C++ with FreeRTOS tasks.

## Goals
- Keep UI smooth (25-30 FPS) without sensor stalls
- Use SRAM for critical data and PSRAM only for large buffers
- Keep the code modular and testable

## Quick Start
- Recommended: PlatformIO (Arduino framework)
- Alternative: Arduino IDE (rename `src/main.cpp` to `.ino` or create a new sketch that includes it)

## Project Structure
- `src/main.cpp`: entry point, tasks, scheduler
- `src/config/`: pins, timing, build config
- `src/models/`: shared state structs
- `src/services/`: high-level services (IMU, meteo, GNSS, LoRa, UI)
- `src/drivers/`: low-level sensor wrappers (stubs for now)
- `src/ui/`: screens and rendering logic (stubs for now)
- `docs/ARCHITECTURE.md`: system plan and data flow
- `docs/BSEC_NOTES.md`: notes for BME688 IAQ (BSEC) integration

## Build Notes
- This skeleton compiles only after adding the required libraries.
- Suggested libraries:
  - LVGL (UI)
  - TinyGPS++ (GNSS)
  - Madgwick or Mahony filter
  - Sensor drivers for BMP581, BMM350, BME688, LTR-390, QMI8658

## Next Steps
1. Confirm pin map in `src/config/pins.h`
2. Add sensor drivers and verify I2C addresses
3. Connect UI widgets to `SystemState`
4. Add LoRa packet format and send rate

If you want, I can also add a PlatformIO `platformio.ini` and basic build presets.
