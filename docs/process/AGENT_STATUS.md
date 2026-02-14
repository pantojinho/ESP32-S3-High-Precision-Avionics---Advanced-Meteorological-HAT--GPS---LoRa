# Agent Status Board

Last update: 2026-02-14
Phase: v1 baseline implementation + UI input + mesh/gps mode integration

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
| FW-004 | P1 | DONE | Build mini-OS UI baseline | Multi-page UI rendered with mode navigation + comms/status cards |
| FW-005 | P1 | DONE | Integrate touch input + UI actions | FT3168 touch is read by LVGL and UI controls are clickable |
| FW-006 | P1 | DONE | Add GPS mission mode | Dedicated GPS page renders fix/sat/HDOP/course/age |
| FW-007 | P1 | DONE | Add MeshLoRa baseline | Mesh frame format + TX/RX/forward counters integrated in LoRa service |
| FW-008 | P1 | OPEN | Field-validate MeshLoRa + GNSS | Real hardware test with at least 2 nodes and live GPS feed |
| FW-009 | P1 | DONE | Document Mini-OS sensor test plan | UI doc contains objective sequence, acceptance criteria, and evidence checklist |

## Open risks

1. Real hardware validation pending (no board-level test yet).
2. `pio` tooling missing in current environment, build not executed.
3. Mechanical STEP currently placeholder until CAD export.
4. RadioLib/TinyGPS integration was not compiled in this environment (toolchain unavailable).
5. Mesh forwarding logic needs multi-node field validation to tune TTL/retry behavior.

## Session log

### 2026-02-11

- Implemented repository structure for v1 plan.
- Archived `Projeto Antigo` into `archive/legacy/Projeto_Antigo`.
- Created hardware and process docs for manufacturing and pin map.
- Updated firmware model/services/tasks to align with v1 architecture.
- Validation note: `pio run` failed because PlatformIO CLI is not installed in this environment.

### 2026-02-14

- Task executed: FW-004 mini-OS UI baseline for display firmware.
- Acceptance criteria (objective):
  - six operational pages available: Home, Avionics, Meteo, Tracker, Black Box, Config
  - top status bar with connectivity and battery indicators
  - bottom navigation and mode selector present in UI
  - each page bound to `SystemState` telemetry fields
- Files changed:
  - `src/ui/mini_os.h`
  - `src/ui/mini_os.cpp`
  - `src/services/services.h`
  - `src/services/services.cpp`
  - `src/models/state.h`
  - `src/main.cpp`
  - `src/drivers/display.h`
  - `src/drivers/display.cpp`
  - `docs/software/IMPLEMENTATION_PHASES.md`
  - `docs/software/UI_MINI_OS_DESIGN.md`
- Decisions:
  - UI architecture classified as `CANDIDATE` (software-level, no PCB impact)
  - mini-OS uses card layout + mode pages with auto-cycle fallback while touch is pending
  - icon strategy standardized on LVGL symbols with text fallback
- Blockers:
  - local environment still lacks PlatformIO CLI (`pio`/`platformio` not found), so build was not executed here
  - touch driver integration pending
- Evidence:
  - static inspection of all edited files
  - command evidence: `pio --version` and `platformio --version` failed in this environment
- Next step:
  - implement FW-005 by wiring touch controller input and binding config toggles to real communication services

### 2026-02-14 (session 2)

- Task executed: touch driver + video/display driver refinements + MeshLoRa baseline + GPS mode.
- Acceptance criteria (objective):
  - FT3168 touch driver reads coordinates and feeds LVGL indev
  - display driver uses dual DMA buffers and frame pacing controls
  - mini-OS includes dedicated GPS mode page
  - LoRa service emits/reads Mesh frames and tracks forward/drop counters
- Files changed:
  - `src/drivers/touch_ft3168.h`
  - `src/drivers/touch_ft3168.cpp`
  - `src/drivers/display.h`
  - `src/drivers/display.cpp`
  - `src/config/pins.h`
  - `src/models/state.h`
  - `src/services/services.h`
  - `src/services/services.cpp`
  - `src/ui/mini_os.h`
  - `src/ui/mini_os.cpp`
  - `src/main.cpp`
  - `platformio.ini`
  - `.gitignore`
  - `docs/hardware/PINMAP.md`
  - `docs/project/PROJECT_MEMORY.md`
  - `docs/software/UI_MINI_OS_DESIGN.md`
  - `docs/software/IMPLEMENTATION_PHASES.md`
- Decisions:
  - `CANDIDATE`: MeshLoRa firmware support enabled while product requirement remains deferred for locked v1 scope
  - `CANDIDATE`: GPS dedicated mode added to mini-OS navigation model
  - `LOCKED`: FT3168 touch pin usage confirmed on shared I2C (GPIO47/GPIO48), polling mode
- Blockers:
  - local environment still lacks build tooling (`pio`, `platformio`, `arduino-cli`)
- Evidence:
  - reference package inspected: `tools/waveshare_demo_1p64/.../06_LVGL_Test/FT3168.*` and `lcd_config.h`
  - static inspection of all changed firmware files
- Next step:
  - run build + upload on hardware and validate touch calibration, GNSS lock, and 2-node mesh relay behavior

### 2026-02-14 (session 3)

- Task executed: documented mini-OS next steps focused on sensor validation flow.
- Acceptance criteria (objective):
  - mini-OS document includes sensor test matrix by page
  - execution sequence includes time-bounded phases
  - acceptance and evidence checklist explicitly defined
- Files changed:
  - `docs/software/UI_MINI_OS_DESIGN.md`
  - `docs/process/AGENT_STATUS.md`
- Decisions:
  - `CANDIDATE`: mini-OS becomes the primary HMI for validating sensor drivers in field tests
- Blockers:
  - no hardware run was executed in this session
- Evidence:
  - static inspection of updated documentation sections
- Next step:
  - execute FW-008 on hardware and attach evidence logs/media to status board
