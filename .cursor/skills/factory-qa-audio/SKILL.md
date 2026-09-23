---
name: factory-qa-audio
description: "Mandatory QA subagent workflow for sound-factory batches: clipping, silence, duration gates. Use after run_product_batch and before PR or MPC export."
---

# Factory QA (subagent)

## When to invoke

After **`run_product_batch.py`** or any multisample render. Treat as a **QA subagent** pass (Cursor `debug` or dedicated agent turn with this skill).

## Commands

```bash
cd disklordz/sound-factory/scripts
python3 factory_qa.py ../out/JZ400/**/samples/*.wav --json-out ../out/JZ400/qa-report.json --fail-fast
```

## Fail closed (QA v2)

| Reason | Action |
|--------|--------|
| `clipping` | Re-render lower drive or reject slot |
| `near_silence` / `too_quiet` | Re-render or drop slot |
| `harsh_crest` / `harsh_slope` | Lower drive / filter FM or best-of-N |
| `mono_sub_only` | Wrong archetype — re-pick program |
| `too_short` / `too_long` | Fix duration in generator |

## Best-of-N (optional)

Use `best-of-n-runner` with seeds N..N+2; keep highest RMS without clipping.

## PR evidence

Attach `qa-report.json` + `batch_report_*.json`. Do not commit failing WAVs.
