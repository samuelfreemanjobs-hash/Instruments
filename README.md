# JD Upgraded

Clean-room JUCE VST3/standalone synthesizer inspired by the Roland JD-800 / JD-990 architecture: four tones per voice, PCM waveforms, rate/level envelopes, and (upcoming) TVF, modulation, and multi-effects modeling.

**Legal**: Factory sound comes from **`jdupg_cleanroom.rom`** — 128 procedurally synthesized waves (see [docs/ROM.md](docs/ROM.md)). No Roland ROM is bundled. Optional user dump loading may follow in a later phase.

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

Targets: `JDUpgraded_VST3`, `JDUpgraded_Standalone`.

## Status

| Phase | Scope | Status |
|-------|--------|--------|
| 1 | Audio pipeline, voice pool, sample engine, 32-sample envelopes | Done |
| 2 | ZDF filter, sync/ring/PCM, Group A/B effects | Done |
| 3 | 256-wave ROM, 128 factory 4-tone programs, per-tone mute | In progress |
| 4 | SIMD, full UI, hardware A/B | Planned |

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).
