---
name: apc-ship
description: APC Ship — professional package (versioned zip, INSTALL.txt) under release/
---

# Ship phase (`/apc-ship [Name]`)

**A plugin is not complete until Ship runs.** Local `build/` alone is not a release.

## Prerequisites

- `/apc-test` passed (VST3 exists)
- Optional gate: `bash tools/apc-gate.sh <Name> ship`

## Package

From repo root:

```bash
./tools/package-vst.sh <Name> [version]
```

Default version `1.0.0` or read from `plugins/<Name>/status.json` → `version`.

## Outputs

- `release/<Name>/<Name>-<version>-vst3.zip`
- Contains: `.vst3` bundle, `INSTALL.txt`, `manifest.json`

## Agent tasks

1. Run package script (build if needed).
2. Update `plugins/<Name>/README.md` with install path for Linux/Windows/macOS.
3. Set `validation.ship_ready` = true in `status.json`.
4. Summarize zip path for the user.

## Professional installers

For Windows Inno Setup / macOS signed pkg, see `packaging/README.md` — extend when targeting those stores.
