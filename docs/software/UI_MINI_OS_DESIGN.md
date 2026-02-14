# UI Mini-OS Design Strategy

Last update: 2026-02-14
Status: CANDIDATE (software baseline)

## Goal

Define a coherent "mini-OS" visual layer for ESP32-S3 avionics firmware, covering:
- navigation menu
- mission modes
- configuration flows
- connectivity visibility (Wi-Fi, BLE, LoRa, GNSS)
- local/online server status

## Visual Direction

- Theme: cockpit + telemetry (deep blue background, cyan/green status accents, amber warnings).
- Layout: 3 horizontal zones:
  - top status bar (time, links, battery)
  - central page (mission content)
  - bottom nav bar (prev/mode/next)
- Density: card-based layout with high-contrast labels for sunlight readability.

## Mini-OS Information Architecture

Pages:
1. Home
2. Avionics
3. Meteo
4. Tracker
5. GPS
6. Black Box
7. Config

Navigation:
- touch buttons (`Prev` / `Next`) in nav bar
- mode dropdown in `Config`
- FT3168 touch controller integrated as LVGL pointer input
- auto-cycle fallback when no interaction is detected

## Modes and Widgets

### 1) Home
- Mission profile summary
- Connectivity matrix (Wi-Fi/BLE/LoRa/GNSS)
- Quick operation hints

### 2) Avionics
- Attitude (pitch/roll/yaw)
- Altimetry (altitude + vario)
- Pressure + flight health status

### 3) Meteo
- Temperature / humidity / pressure
- Gas channel (BME688 resistance)
- Meteo health flags

### 4) Tracker
- GNSS fix state + satellites
- Lat/Lon + fix age
- LoRa telemetry + Mesh counters (forwarded/dropped)

### 5) GPS
- dedicated fix monitor (fix type, satellites, HDOP, age)
- navigation vector (speed, course, altitude)
- signal quality panel with geospatial validity

### 6) Black Box
- Logger/storage state
- Fault counters by subsystem
- Last telemetry payload preview

### 7) Config
- Mode selector
- Connectivity toggles:
  - Wi-Fi
  - Bluetooth LE
  - LoRa
  - GNSS
  - Web server
- Server profile (online/offline, port, clients)

## Icon Strategy (LVGL symbols)

| Context | Icon |
| --- | --- |
| Home | `LV_SYMBOL_HOME` |
| Avionics | `LV_SYMBOL_CHARGE` |
| Meteo | `LV_SYMBOL_TINT` |
| Tracker | `LV_SYMBOL_GPS` |
| GPS | `LV_SYMBOL_GPS` |
| Black Box | `LV_SYMBOL_SAVE` |
| Config | `LV_SYMBOL_SETTINGS` |
| Wi-Fi | `LV_SYMBOL_WIFI` |
| BLE | `LV_SYMBOL_BLUETOOTH` |
| Battery | `LV_SYMBOL_BATTERY_*` |

Fallback rule:
- If a symbol is unavailable in a custom font build, replace by short labels (`HOME`, `AV`, `MET`, `TRK`, `BOX`, `CFG`).

## Online Server UX Strategy

Local channels exposed by ESP:
- Wi-Fi AP/STA for web dashboard
- BLE for quick configuration/bootstrap
- LoRa for telemetry uplink/downlink
- GNSS for geospatial context

Server states shown in UI:
- `OFFLINE`: service disabled or not connected
- `LOCAL`: web server active in local network/AP mode
- `SYNC`: connected to upstream/cloud bridge
- `FAULT`: retries/fault counter above threshold

Minimum telemetry surface for web dashboard:
- `/api/state`: compact JSON snapshot (flight + meteo + gnss + radio + health)
- `/api/config`: current mode and comm toggles
- `/api/logs`: rolling last-N records status

## HMI Safety Rules

- Never hide subsystem fault counters.
- Keep battery and link status always visible in status bar.
- No single page should require scrolling for mission-critical values.
- UI must remain readable even when touch input fails (auto-cycle + fixed nav).

## Implementation Baseline in Firmware

Files:
- `src/ui/mini_os.h`
- `src/ui/mini_os.cpp`
- `src/services/services.cpp`
- `src/models/state.h`

Current baseline behavior:
- live multi-page mini-OS rendering from `SystemState`
- mission mode visualization and config menu structure
- FT3168 touch input integrated into LVGL
- GNSS mode integrated with TinyGPS++ fields
- MeshLoRa frame format implemented (`M1|src|dst|ttl|seq|payload`)

## Next Steps - Sensor Test Plan (Mini-OS Driven)

Objective:
- validate real sensor drivers using mini-OS pages as the primary HMI.

Test matrix:
- `Avionics` page:
  - QMI8658: pitch/roll/yaw must change with board movement.
  - BMP581: altitude and pressure must react coherently to vertical displacement.
- `Meteo` page:
  - BME688: temperature/humidity/gas values must update without freezing.
- `GPS` page:
  - GNSS: fix type, satellites, HDOP, course, and fix age must be consistent in open-sky test.
- `Tracker` page:
  - LoRa + GNSS combined telemetry must update packet counters and coordinates.
- `Black Box` page:
  - health/fault counters must reflect real subsystem errors.

Execution sequence:
1. Boot board and wait 60 s warm-up.
2. Run `Avionics` dynamic movement test for 5 min.
3. Run `Meteo` stability test for 10 min (values should remain plausible).
4. Run `GPS` outdoor lock test for 15 min (monitor satellites/HDOP/fix age).
5. Run `Tracker` test with LoRa peer for 15 min (TX/RX and mesh counters).
6. Confirm `Black Box` counters and final payload snapshot.

Acceptance criteria:
- no UI freeze or watchdog reset during complete sequence.
- all tested sensors show live variation in expected ranges.
- GNSS reaches stable fix with finite fix age and non-zero satellites.
- LoRa TX increases continuously; RX/mesh counters move when peer traffic exists.

Evidence to record:
- photos/videos for each page in test mode.
- serial log snippets with timestamp for each phase.
- final checklist entry in `docs/process/AGENT_STATUS.md`.

## Next UI Steps

1. Bind config toggles to real services (Wi-Fi/BLE/LoRa power state changes).
2. Add alert overlays (low battery, GNSS lost, LoRa timeout).
3. Expose the same mode/config model to web server endpoints.
