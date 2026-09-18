# WAVE-909 QA Report (v0.1.0)

## Automated

| Check | Status |
|-------|--------|
| `Wave909Tests` | VERIFIED — wavetable, engine render, filter impulse |
| `Wave909StateTests` | VERIFIED — state + program index |
| `Wave909OfflineRender` A/B | VERIFIED — deterministic render |
| Golden WAV suite | VERIFIED — 3 presets in `tests/golden/` |
| CI (`build.yml`) | VERIFIED — Wave909 targets + ctest + golden |

## Manual

| Check | Status |
|-------|--------|
| Standalone UI | VERIFIED — presets, destructive circuit |
| DAW load | NOT VERIFIED |

## Known limitations

- User wavetable import (P1).
- CPU profiling (P1).
