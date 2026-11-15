# Disklordz Audio — TRAP LORD SYNTH

Hybrid Southern-trap ROM-style instrument (VST3 + Standalone). JUCE is fetched by CMake (pinned tag from repo `JUCE_VERSION`, minimum 8.0.0).

## Build

### Linux

```bash
cd disklordz/trap-lord-synth
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target TrapLordSynth_VST3 TrapLordSynth_Standalone
```

### Windows (Visual Studio 2022)

```powershell
cmake -S disklordz/trap-lord-synth -B disklordz/trap-lord-synth/build `
  -G "Visual Studio 17 2022" -A x64
cmake --build disklordz/trap-lord-synth/build --config Release `
  --target TrapLordSynth_VST3 TrapLordSynth_Standalone
```

Artefacts: `build/TrapLordSynth_artefacts/Release/VST3/` and `Standalone/`.

## ATL Classics sound bank

Eighteen neutral-named factory presets across **Church Keys**, **Dark Brass**, and **Trap Orchestra**, plus macros **Darkness**, **Dirt**, **Movement**, and **Space**. Details: `docs/ATL_CLASSICS.md`. Demo melody: `assets/demo_f_harmonic_minor_140bpm.mid`.

## Validation

From repo root:

```bash
python3 scripts/vst/run_pluginval.py --discover disklordz/trap-lord-synth/build/TrapLordSynth_artefacts/Release/VST3 \
  --strictness 5
```

See `docs/ROADMAP.md` for milestone status and `ARCHITECTURE.md` for module map.
