# ESP32-S3 Waveshare HAT v1 (Telemetry + Flight + Balloon)

PT-BR:
Este repositorio contem o firmware e a documentacao para uma HAT PCB que encaixa por baixo da
Waveshare ESP32-S3 Touch AMOLED 1.64. O foco da v1 e: LoRa 915 MHz, GPS externo, sensores de voo
(IMU + altimetria) e sensores meteo para modo balao.

EN:
This repository contains firmware and docs for a bottom-mounted HAT PCB for the Waveshare
ESP32-S3 Touch AMOLED 1.64. v1 focus: LoRa 915 MHz, external GPS, flight sensors (IMU + altimetry),
and meteo sensors for balloon mode.

## Locked v1 Scope

- LoRa SX1262/CC68 (915 MHz, telemetry RX/TX)
- IMU QMI8658 (attitude baseline)
- BMP581 (altitude + vario)
- BME688 (temperature/humidity/gas)
- GPS external connector (1x6, 2.54 mm)
- Shared I2C expansion header
- Powered from Waveshare BAT/3V3 rails

## Repository Layout

- `src/`: active firmware skeleton and task model
- `hardware/hat_v1/`: mechanical/electrical/manufacturing artifacts for v1
- `docs/hardware/`: pin map, requirements, power budget, manufacturing package
- `docs/software/`: implementation phases and bring-up flow
- `docs/project/`: project memory and file inventory
- `docs/process/`: status board and collaboration state
- `archive/legacy/`: archived legacy references (migrated from old project folder)
- `AGENTS.md`: handoff and update protocol for agents

## Start Here

1. Hardware baseline: `docs/hardware/PINMAP.md`
2. Requirements: `docs/hardware/HAT_V1_REQUIREMENTS.md`
3. Phases: `docs/software/IMPLEMENTATION_PHASES.md`
4. Memory and decisions: `docs/project/PROJECT_MEMORY.md`
5. Live status: `docs/process/AGENT_STATUS.md`

## Build Notes

- Framework: Arduino + FreeRTOS on ESP32-S3
- Current firmware is an integration skeleton with non-blocking services
- Recommended libs for full implementation:
  - TinyGPS++
  - RadioLib
  - BMP581/QMI8658/BME688 drivers
  - Optional UI stack (LVGL)
