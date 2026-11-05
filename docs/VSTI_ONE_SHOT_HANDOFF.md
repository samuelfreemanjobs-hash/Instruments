# VSTi one-shot style pack — project handoff

**Product:** `tools/vsti-one-shot/` — **Boom** pipeline for Akai MPC melodic one-shots from any VST3 factory bank.

**Status:** Complete for v1.0 local/studio use.

## Start (studio)

**Windows (primary):** [docs/VSTI_ONE_SHOT_WINDOWS.md](VSTI_ONE_SHOT_WINDOWS.md)

```powershell
.\scripts\boom.ps1 `
  -Style "YOUR STYLE BRIEF" `
  -Vst3 "C:\Program Files\Common Files\VST3\Serum.vst3" `
  -Out "$env:USERPROFILE\Exports\mpc_pack"
```

**Linux / macOS:**

```bash
./scripts/boom.sh --style "YOUR STYLE" --vst3 "/path/to/Plugin.vst3" --out ~/Exports/mpc_pack
```

## Definition of done (v1.0)

| Item | Location |
|------|----------|
| User entry | `scripts/boom.ps1` (Windows), `scripts/boom.sh`, `tools/vsti-one-shot/boom.py` |
| Recipe Author (100 slots) | `vsti_one_shot/recipe_author.py` |
| Preset Matcher (hybrid default) | `vsti_one_shot/matcher.py`, `preview_analyze.py` |
| Render Executor (strict) | `build/VstiOneShotRender` |
| Recipe book + agent SOPs | `tools/vsti-one-shot/docs/` |
| Job schema | `tools/vsti-one-shot/schema/job.schema.json` |
| Architecture | `tools/vsti-one-shot/ARCHITECTURE.md` |
| CI | `.github/workflows/vsti-one-shot.yml` |
| Tests | `python3 -m pytest tools/vsti-one-shot/tests -v` |

## Outputs

- `{slot_id}.wav` × N (default 100), mono, 44.1 kHz, trimmed, peak-normalized
- `manifest.json` — program index, duration, paths
- `.boom_<utc>/` — `recipe.json`, `assignments.json`, `job.json` audit trail

## Matcher modes

- **`hybrid`** (default) — name + 1.5 s preview scoring + reuse spread
- **`names`** — fast, metadata only
- **`preview`** — audio scoring only

## Studio notes

- Run on **Mac/Windows** with your commercial VST3 paths.
- Hybrid matching adds time before the final batch (~8 previews × slot count).
- Render executor never changes matcher choices; re-run from saved `job.json` for identical WAVs.

## Related

- [tools/vsti-one-shot/docs/SAMPLE_RECIPE_BOOK.md](../tools/vsti-one-shot/docs/SAMPLE_RECIPE_BOOK.md)
- [tools/ARCHITECTURE.md](../tools/ARCHITECTURE.md)
