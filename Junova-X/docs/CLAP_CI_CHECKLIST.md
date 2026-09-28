# Junova-X CLAP + CI checklist (WO-2026-002)

## Build targets

```bash
cmake --build build -j --target JunovaX_VST3 JunovaX_CLAP JunovaX_Standalone
```

## Artefacts (Release)

| Format | Path |
|--------|------|
| VST3 | `build/Junova-X/JunovaX_artefacts/Release/VST3/Junova-X.vst3` |
| CLAP | `build/Junova-X/JunovaX_artefacts/Release/CLAP/Junova-X.clap` |
| Standalone | `build/Junova-X/JunovaX_artefacts/Release/Standalone/Junova-X` |

## CI

- Root [`.github/workflows/build.yml`](../../.github/workflows/build.yml) full monorepo build includes Junova-X via `add_subdirectory(Junova-X)`.
- [vst-testing-ops/business_pipeline.py](../../vst-testing-ops/business_pipeline.py) checks Junova artefacts and runs pluginval on `Junova-X.vst3`.

## Host smoke (manual)

1. Load VST3 and CLAP in Reaper (or Bitwig CLAP).
2. MIDI input → audible poly synth; preset prev/next in Celestial UI.
3. Save/recall project — state restores preset + parameters.

See [QA_HOST_SMOKE.md](QA_HOST_SMOKE.md).
