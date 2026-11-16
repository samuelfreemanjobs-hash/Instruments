---
name: ddl-standalone-build
description: >-
  Configures and builds DiskLordz Drum Design Lab JUCE Standalone (and optional
  VST3) on Windows with VS2022. Use when the user asks for standalone app,
  DrumDesignLab.exe, or JUCE plugin build for DDL.
---

# DDL Standalone Build (Windows)

## Prerequisites

- VS 2022 C++ tools, CMake 4.x
- JUCE at `C:/Users/user/Downloads/juce-9.0.2-windows/JUCE` (or `-DJUCE_DIR=...`)
- Worktree: prefer `C:\dev\Instruments-drum-design-lab` — do not switch `C:\dev\Instruments` branch
- One heavy build at a time (no parallel `npm` / second C++ build)

## CMake requirements (JUCE 9)

In `vst/DrumDesignLab/CMakeLists.txt`:

- `project(... VERSION x.y.z LANGUAGES C CXX)` — **C** required; **VERSION** required
- `DISKLORDZ_BUILD_JUCE_PLUGIN=ON`
- `juce_generate_juce_header(DrumDesignLab)` if sources `#include <JuceHeader.h>`
- `COPY_PLUGIN_AFTER_BUILD FALSE` (never Program Files VST3)

## Commands

```powershell
cd vst/DrumDesignLab
cmake -S . -B build-juce -G "Visual Studio 17 2022" -A x64 `
  -DDISKLORDZ_BUILD_JUCE_PLUGIN=ON `
  -DJUCE_DIR="C:/Users/user/Downloads/juce-9.0.2-windows/JUCE"

# Standalone only (faster than building VST3 too)
$vs = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
cmd /c "`"$vs`" && cmake --build build-juce --config Release --target DrumDesignLab_Standalone -- /v:minimal /m:2"
```

## Artifact

```text
vst/DrumDesignLab/build-juce/DrumDesignLab_artefacts/Release/Standalone/DiskLordz Drum Design Lab.exe
```

Cold build ≈ 30–60+ minutes. Do not claim success without this path existing.

## Core-only (no JUCE)

```powershell
cmake -S . -B build-core -G "Visual Studio 17 2022" -A x64 -DDISKLORDZ_BUILD_JUCE_PLUGIN=OFF
cmake --build build-core --config Release
ctest --test-dir build-core -C Release --output-on-failure
```

## Common failures

| Error | Fix |
|-------|-----|
| A C compiler is required | Add `C` to `LANGUAGES` |
| VERSION argument set / PROJECT_VERSION | Set `project(... VERSION 0.1.0 ...)` |
| Cannot open `JuceHeader.h` | `juce_generate_juce_header(DrumDesignLab)` |
| Device Guard blocked .exe | Copy to alternate name (e.g. `ddl_batch.exe`) for tools |

## After build

- Do **not** auto-install to DAW folder
- Smoke: launch Standalone, pick Kit Style, MIDI C1–D#2 pads
