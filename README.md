# JD Upgraded

Clean-room JUCE VST3/standalone synthesizer inspired by the Roland JD-800 / JD-990 architecture: four tones per voice, PCM waveforms, rate/level envelopes, and (upcoming) TVF, modulation, and multi-effects modeling.

**Legal**: This repository does not include proprietary Roland ROM or SysEx bank data. Phase 3 adds a user-supplied ROM loader and optional cleanroom waveforms only.

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
| 3 | ROM loader, SysEx presets | Planned |
| 4 | SIMD, full UI, hardware A/B | Planned |

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).
