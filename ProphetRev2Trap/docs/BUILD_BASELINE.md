# Night Circuit — reproducible build baseline (Gate 1)

**Status:** Canonical CI/dev baseline for verification. Feature work stays behind this gate.

## Pinned toolchain

| Component | Pin |
|-----------|-----|
| **JUCE** | **8.0.15** (`FetchContent` in repo root [CMakeLists.txt](../../CMakeLists.txt)) |
| **C++** | 20 |
| **Compiler (Linux CI / Cloud)** | **gcc-12 / g++-12** |
| **OS reference** | Ubuntu 24.04 (glibc 2.39), x86_64 |
| **CMake target** | `ProphetRev2Trap` |
| **Product / VST3 name** | **Night Circuit** |
| **Ship format** | **VST3** (desktop). Standalone for dev smoke only. |

Resolved commit for tag **8.0.15** (example from CI): `91ad83ae34a81e0833b1a2b0866f54846370ae53`

```bash
git -C build/_deps/juce-src rev-parse HEAD
```

## Build commands

```bash
cd /path/to/Instruments
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target ProphetRev2Trap_VST3 ProphetRev2Trap_Standalone NightCircuitTests
```

## Artifacts

| Output | Path |
|--------|------|
| VST3 | `build/ProphetRev2Trap/ProphetRev2Trap_artefacts/Release/VST3/Night Circuit.vst3` |
| Standalone | `build/ProphetRev2Trap/ProphetRev2Trap_artefacts/Release/Standalone/Night Circuit` |
| Tests | `build/ProphetRev2Trap/NightCircuitTests` |

## Gate 1 automated checks

```bash
./build/ProphetRev2Trap/NightCircuitTests
python3 ProphetRev2Trap/qa/verify_preset_bank.py
xvfb-run -a python3 scripts/vst/run_pluginval.py \
  --plugin "build/ProphetRev2Trap/ProphetRev2Trap_artefacts/Release/VST3/Night Circuit.vst3"
```

`NightCircuitTests` covers: analog helpers, **1,028** preset bank quotas/uniqueness/ranges, **MIDI note-on → non-silent audio**, **state blob round-trip**.

## Gate 1 manual host check

1. Launch **Standalone** (or load VST3 in a DAW on your platform).
2. Confirm editor opens at default width without overlapping top-row preset controls (minimum width **960** px).
3. Play MIDI (middle C, velocity ~100); confirm audible output and note-off release.

Log results in [../qa/reports/](../qa/reports/).

## Target hosts (product)

**Primary DAW sign-off:** **FL Studio** and **Akai MPC Software** (desktop application — not MPC standalone hardware as a VST3 host).  
Track exact versions and OS in [HOST_COMPATIBILITY.md](HOST_COMPATIBILITY.md).

| Layer | Choice | Rationale |
|-------|--------|-----------|
| **Build truth / Cursor VM** | Linux + g++-12 + JUCE 8.0.15 | Compile, tests, pluginval — **not** FL/MPC compatibility proof |
| **Dev smoke** | JUCE Standalone (Linux) | Same DSP as VST3; no DAW |
| **Musical + host QA** | FL Studio & MPC Software on **Windows/macOS** | Real plugin scan, editor, MIDI, project recall |
| **Phase 5** | Installers, signing/notarization | After Gates 1–2 green |

## Related

- QA agent: [../../docs/NIGHT_CIRCUIT_QA_SUBAGENT.md](../../docs/NIGHT_CIRCUIT_QA_SUBAGENT.md)
- Verification roadmap: [../../docs/NIGHT_CIRCUIT_VERIFICATION.md](../../docs/NIGHT_CIRCUIT_VERIFICATION.md)
- Sound-design agent: [../../docs/NIGHT_CIRCUIT_SOUND_DESIGN_SUBAGENT.md](../../docs/NIGHT_CIRCUIT_SOUND_DESIGN_SUBAGENT.md)
- Host matrix: [HOST_COMPATIBILITY.md](HOST_COMPATIBILITY.md)
- Preset GUI: [PRESET_BROWSER_GUI.md](PRESET_BROWSER_GUI.md)
