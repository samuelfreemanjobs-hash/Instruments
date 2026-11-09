# DSP knowledge index

Curated pointers for factory agents and Cursor implementers. Prefer shared portable headers and factory-lib stubs over inventing new DSP.

## Sources of truth

| Layer | Path | Role |
|-------|------|------|
| Portable float DSP | `shared/dsp/` | JUCE-free helpers (biquad, ADSR, smoothing, phasor) |
| Factory C++ stubs | `vst/factory-lib/` | Catalog-backed blocks referenced by `modules/dsp/*.json` |
| Architecture law | `docs/dsp-architecture-specification.md` | Realtime / threading rules |
| Module catalog | `modules/dsp/*.json` | IDs used in factory-spec `dsp_modules` |

## How to extend

1. Add or implement a block under `vst/factory-lib/` (or reuse `shared/dsp/`).
2. Register it in the matching `modules/dsp/*.json` file.
3. Add an entry to `index.json` in this folder.
4. Reference the module id from taxonomy / `modules/features.json`.

Machine-readable index: [`index.json`](index.json).
