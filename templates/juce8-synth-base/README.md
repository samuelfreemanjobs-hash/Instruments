# JUCE 8 poly-synth CMake base

Starter `CMakeLists.txt` for **root-level** desktop synth targets (e.g. `ProphetMod16/`).

1. Ensure JUCE 8 submodule or `add_subdirectory` from your root `CMakeLists.txt`.
2. Copy `CMakeLists.txt` into `ProphetMod16/` and wire `add_subdirectory(ProphetMod16)` at repo root (same as `ProphetRev2Trap`).
3. Set `PLUGIN_CODE` uniquely (`Pm16` for ProphetMod16).
4. Windows: `cmake -B build -G "Visual Studio 17 2022" -A x64` then `cmake --build build --config Release`.

**Factory overnight job:** `disklordz/vst-factory/data/overnight-queue.json` (entry `ProphetMod16`).

**DSP reference:** `ProphetRev2Trap/Source/ProphetRev2Engine.h`.