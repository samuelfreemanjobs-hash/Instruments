---
name: plugin-build-verify
description: Builds VST plugins with cmake, checks artifacts, and reports toolchain gaps. Use after DSP/UI changes or when user cannot build locally.
---

# Plugin Build Verify subagent

## Prerequisites

- `vst/JUCE` submodule initialized
- Linux: `build-essential`, `cmake`
- ARM gcc **not** required for VST (host compiler)

## Commands

From repo root:

```bash
export LOGUE_SDK=...   # only for logue cross-builds
cd vst/<Plugin>
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Optional wrapper (when present):

```bash
./tools/build-vst.sh <Plugin>
```

## Success criteria

- CMake configures without error
- Target links
- Artifact exists, e.g.:
  - `build/<Plugin>_artefacts/Release/VST3/<Plugin>.vst3`

## On failure

Report:

1. Exact compiler error (last 30 lines)
2. Missing dependency (JUCE, cmake, libasound for Linux standalone)
3. Suggest Docker if user on Windows without WSL build chain

## mkII / logue

For hardware units use:

```bash
./tools/build-mkii.sh <slug>
./tools/build.sh oscillators/<unit> nutekt-digital
```

Document if `LOGUE_SDK` or gcc missing; do not claim success without artifact.

## Deliverables

- Build log summary
- Path to `.vst3` or `.nts1mkiiunit`
- List of warnings worth fixing (LTO, deprecated JUCE APIs)
