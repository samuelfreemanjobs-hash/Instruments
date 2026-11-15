# JUCE 8 poly-synth CMake base

Starter `CMakeLists.txt` for **root-level** desktop synth targets (e.g. `ProphetMod16/`).

1. Ensure JUCE 8 submodule or `add_subdirectory` from your root `CMakeLists.txt`.
2. Copy `CMakeLists.txt` into `ProphetMod16/` and wire `add_subdirectory(ProphetMod16)` at repo root (same as `ProphetRev2Trap`).
3. Set `PLUGIN_CODE` uniquely (`Pm16` for ProphetMod16).
4. Windows: `cmake -B build -G "Visual Studio 17 2022" -A x64` then `cmake --build build --config Release`.

**Factory overnight batch:** `disklordz/vst-factory/data/overnight-queue.json`

| Plugin | CODE | Voices | Notes |
|--------|------|--------|--------|
| ProphetMod16 | Pm16 | 16 | Rev2 mod matrix; fork `ProphetRev2Trap` engine |
| OxfordPeak8 | Opk8 | 8 | 3 osc wavetable + dual-source 16-slot matrix |
| PlaySynthDreams | PsDr | 12 | Dual-layer Play Series + 8 macros |
| PlaySynthAnalogDreams | PsAD | 12 | Engine fork, amber UI |
| PlaySynthCloudSupply | PsCS | 12 | Engine fork, ice UI |
| PlaySynthLoFiGlow | PsLF | 12 | Engine fork, tape UI |

Copy `CMakeLists.txt`, rename project/plugin symbols; see `CMakeLists.OxfordPeak8.example.txt` for renames.

Factory presets: wire `BinaryData.cmake.snippet` for `juce_add_binary_data` (no audio-thread file I/O).

**Stubs in same JSON** (priority 4–5): ObXaDesktop, WavestateHybrid, VirusTiDesktop — plan-only until APC design exists. There is no `run-factory.ps1` in-repo; run prompts via Claude Code / Cursor on Windows with MSVC.

**DSP reference:** `ProphetRev2Trap/Source/ProphetRev2Engine.h`.