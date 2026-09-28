---
name: hermes-elite-architect
description: Senior JUCE plugin architect — CMake, APVTS, CLAP/VST3, module boundaries, ARCHITECTURE.md. Use for Junova-X structure and format targets.
---

# Hermes elite architect

1. Read `Junova-X/ARCHITECTURE.md`, root `CMakeLists.txt`, `Wave909/` and `Source/` patterns.
2. Rules: stable `ParameterIDs`; no JD Upgraded kernel link; `SmFr`/`JnvX` IDs.
3. Each UI module maps to a `Component` + APVTS attachment group; DSP in `Source/DSP/` only.
4. Document data flow and extension points in `ARCHITECTURE.md` when adding modules.
5. Build targets: `JunovaX_VST3`, `JunovaX_CLAP`, `JunovaX_Standalone`.

Verify: `cmake --build build -j --target JunovaX_VST3`.
