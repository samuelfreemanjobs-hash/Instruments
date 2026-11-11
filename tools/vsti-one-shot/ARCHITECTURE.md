# VSTi one-shot style pack — architecture

## Purpose

**Start command:** `boom.py` — you describe a **style**; the pipeline produces **100 raw mono WAV one-shots** from **any VST3** factory bank for **Akai MPC** melodic material (no keygroups).

## Build & run

```bash
cmake --build build -j --target VstiOneShotRender

python3 tools/vsti-one-shot/boom.py \
  --style "dark Memphis melodic stabs" \
  --vst3 "/path/to/Synth.vst3" \
  --out ~/Exports/mpc_pack \
  --match-mode hybrid
```

`--match-mode`: `names` | `preview` | `hybrid` (default). Hybrid runs short preview renders per candidate program and scores audio against slot kind before the final 100 WAV batch.

Dry run (recipe + assignments + job only):

```bash
python3 tools/vsti-one-shot/boom.py --style "..." --vst3 "..." --out /tmp/out --dry-run
```

Tests:

```bash
python3 -m pytest tools/vsti-one-shot/tests -v
```

## Data flow

```text
--style  →  Recipe Author (Python)     →  recipe.json
         →  Preset Matcher (Python)   →  assignments.json  (+ list-programs)
         →  job merge                 →  job.json
         →  VstiOneShotRender (C++)   →  *.wav + manifest.json
```

## Threading / realtime

Offline only. `VstiOneShotRender` sets `setNonRealtime(true)` on the hosted plugin. No audio thread constraints for MPC export.

## Key modules

| Path | Role |
|------|------|
| `boom.py` | User entrypoint |
| `vsti_one_shot/recipe_author.py` | 100 slots from style |
| `vsti_one_shot/matcher.py` | Factory program selection |
| `VstiOneShotRender.cpp` | JUCE VST3 host, MIDI render, trim/normalize |
| `docs/SAMPLE_RECIPE_BOOK.md` | Human + agent guide |
| `docs/agents/*.md` | Per-role SOPs |

## Extension points

- Per-slot `max_capture_sec` in job JSON (supported)
- Add CLAP/AU hosts (compile flags + format loader)
- Matcher: preview-listen loop before full batch
- Optional `.vstpreset` / state blob in assignments

## Related docs

- [docs/SAMPLE_RECIPE_BOOK.md](docs/SAMPLE_RECIPE_BOOK.md)
- [../../tools/ARCHITECTURE.md](../../tools/ARCHITECTURE.md)
- [../../vst-testing-ops/ARCHITECTURE.md](../../vst-testing-ops/ARCHITECTURE.md)
