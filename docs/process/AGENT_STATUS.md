# Agent Status Board

Last update: 2026-02-11
Phase: v1 baseline implementation

## Current objective

Ship a manufacturable HAT v1 baseline for Waveshare ESP32-S3 with consistent
mechanical, electrical, firmware, and documentation artifacts.

## Backlog

| ID | Priority | Status | Task | Done criteria |
| --- | --- | --- | --- | --- |
| HW-001 | P0 | DONE | Consolidate single pin map | `pins.h` and `PINMAP.md` match |
| HW-002 | P0 | DONE | Lock mandatory v1 modules | Requirements doc updated |
| HW-003 | P0 | DONE | Move old project into archive | `archive/legacy/Projeto_Antigo/` exists |
| HW-004 | P1 | OPEN | Add KiCad source for HAT v1 | Schematic + PCB tracked in `hardware/hat_v1/` |
| FW-001 | P1 | DONE | Update SystemState contract | New sections flight/meteo/gnss/radio/power/health |
| FW-002 | P1 | OPEN | Implement real drivers | QMI8658/BMP581/BME688/GNSS/LoRa live data |
| FW-003 | P2 | OPEN | Logger to SD | CSV records with buffered writes |

## Open risks

1. Real hardware validation pending (no board-level test yet).
2. `pio` tooling missing in current environment, build not executed.
3. Mechanical STEP currently placeholder until CAD export.

## Session log

### 2026-02-11

- Implemented repository structure for v1 plan.
- Archived `Projeto Antigo` into `archive/legacy/Projeto_Antigo`.
- Created hardware and process docs for manufacturing and pin map.
- Updated firmware model/services/tasks to align with v1 architecture.
- Validation note: `pio run` failed because PlatformIO CLI is not installed in this environment.
