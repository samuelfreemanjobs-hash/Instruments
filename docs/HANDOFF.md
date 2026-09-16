# Agent handoff — JD Upgraded

Branch: `cursor/jd-synth-phase1-3015` · PR **#6**

## Shipped on this branch

- JUCE VST3/Standalone, 32 voices × 4 tones, zero-alloc audio path
- ZDF TVF, coupling, Group A/B FX, AVX2/NEON tone sum
- ROM v2: 256 waves, 24×8 multisample zones (`GenerateCleanroomRom`)
- 128 factory programs, per-tone mute, program save/restore
- Global + per-tone ADSR, wave palette UI, ROM category metadata
- SysEx: checksum + JD 384-byte patch → APVTS (partial)
- `JDUPGRADED_ROM_PATH` with safe trial load ([USER_ROM.md](USER_ROM.md))
- `OfflineRender` + `SpectralDiff` + CI determinism ([AB_HARNESS.md](AB_HARNESS.md))

## Build

```bash
cmake -B build -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j
```

## Phase 4 checklist

All items below are **done** on this branch:

1. SIMD independent path · 2. Core UI · 3. SysEx (partial) · 4. Global envelopes · 5. Per-tone envelopes · 6. Palette · 7. User ROM dev · 8. A/B harness

## Next

See [PHASE5.md](PHASE5.md). Typical merge follow-up: mark PR ready, tag release, optional `main` CI on merge.

## Key paths

- `Source/PluginProcessor.cpp` — APVTS, MIDI, programs
- `Source/Assets/FactoryPatchLibrary.cpp` — presets
- `tools/GenerateCleanroomRom.cpp` — ROM
- `tools/OfflineRender.cpp`, `tools/SpectralDiff.cpp` — harness
