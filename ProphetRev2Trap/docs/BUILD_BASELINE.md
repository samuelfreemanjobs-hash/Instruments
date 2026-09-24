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

## First target platform (recommended)

| Layer | Choice | Rationale |
|-------|--------|-----------|
| **Build truth** | Linux + g++-12 + JUCE 8.0.15 | Matches Cloud/CI agent, pluginval, fastest feedback |
| **Dev host smoke** | **JUCE Standalone** | Same binary as plugin DSP; no DAW install required |
| **First DAW matrix** | **REAPER** (Linux or macOS on maintainer machine) | Lightweight, strong VST3, common for indie QA |
| **Phase 5** | macOS notarization, Windows installer | After Gate 2 DSP/recall green on Linux |

Adjust when the product owner picks a different primary market (e.g. Logic-first → macOS 14+ / Apple Silicon becomes co-primary).

## Related

- QA agent: [../../docs/NIGHT_CIRCUIT_QA_SUBAGENT.md](../../docs/NIGHT_CIRCUIT_QA_SUBAGENT.md)
- Verification roadmap: [../../docs/NIGHT_CIRCUIT_VERIFICATION.md](../../docs/NIGHT_CIRCUIT_VERIFICATION.md)
- Sound-design ownership: [PRESET_SOUND_DESIGN.md](PRESET_SOUND_DESIGN.md)
