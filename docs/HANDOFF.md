# Agent handoff — JD Upgraded

## Done (branch `cursor/jd-synth-phase1-3015`, PR #6)

- JUCE VST3/Standalone, 32 voices × 4 tones, zero-alloc audio path
- ZDF TVF, coupling, Group A/B FX
- ROM v2: 256 waves, 24×8 multisample zones
- **128 factory programs** (category procedural generator)
- **Per-tone mute** + level sliders in UI
- Program prev/next + name display; **currentProgram** in saved state
- Roland SysEx **checksum validation** stub (`SysExParser`)
- Optional ROM override: `JDUPGRADED_ROM_PATH`

## Build

```bash
cmake -B build -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j
```

## Suggested next (Phase 4 / polish)

1. ~~**SIMD**~~ — `VoiceSimd.h` AVX2 sum of four tone buffers (independent coupling path)
2. ~~**UI (partial)**~~ — filter, Group A/B, coupling, per-tone wave rotaries
3. ~~**SysEx (partial)**~~ — `JdPatchSysexMapper` maps 384-byte JD patch block → APVTS; MIDI SysEx in `processBlock`
4. **UI** — envelope editors, palette link (multisample rotaries added)
5. **User dump loader** — separate legal review; only JDUPGROM in installer
6. **A/B harness** — offline render + spectral diff vs reference clips (user-provided)

## Key paths

- `Source/Assets/FactoryPatchLibrary.cpp` — preset generator
- `tools/GenerateCleanroomRom.cpp` — ROM synthesis
- `Source/PluginProcessor.cpp` — parameters, mute, programs
