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
4. ~~**UI (envelopes)**~~ — global Amp/Filter ADSR in APVTS + editor (shared across 4 tones)
5. ~~**UI (per-tone env)**~~ — `toneN*` ADSR params, **Link** toggle, tone selector in editor
6. ~~**UI (palette)**~~ — category colors, palette snap combo, wave labels from ROM metadata
7. ~~**User ROM (dev)**~~ — safe `JDUPGRADED_ROM_PATH` load + `docs/USER_ROM.md`; Roland dumps rejected by format
8. ~~**A/B harness**~~ — `OfflineRender` + `SpectralDiff` CLI; CI determinism check

## Key paths

- `Source/Assets/FactoryPatchLibrary.cpp` — preset generator
- `tools/GenerateCleanroomRom.cpp` — ROM synthesis
- `Source/PluginProcessor.cpp` — parameters, mute, programs
