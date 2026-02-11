# File Inventory and Classification

Last update: 2026-02-11

## Summary

| Top-level path | Files | Classification |
| --- | --- | --- |
| `archive/` | 187 | Legacy/reference |
| `src/` | 6 | Active firmware |
| `docs/` | 16 | Active documentation |
| `hardware/` | 8 | Active HAT v1 artifacts |
| `examples/` | 1 | Active bring-up reference |
| Root files (`README.md`, `AGENTS.md`, `platformio.ini`) | 3 | Active control files |

## Active paths (v1)

1. Firmware:
- `src/`
- `examples/bringup/`

2. Hardware:
- `hardware/hat_v1/`
- `docs/hardware/`

3. Process/project:
- `docs/project/`
- `docs/process/`
- `docs/software/`

## Legacy archive

Original folder `Projeto Antigo/` was migrated to:
- `archive/legacy/Projeto_Antigo/`

Reason:
- preserve historical files while keeping the active repository clean.

## Cleanup policy applied

1. Keep legacy content available for traceability.
2. Keep active work only in `src/`, `hardware/hat_v1/`, and new docs structure.
3. New artifacts must not be added to `archive/`.
