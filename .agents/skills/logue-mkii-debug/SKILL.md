---
name: logue-mkii-debug
description: Diagnose mkII build or audio issues (/logue-mkii-debug).
---

# Debug (`/logue-mkii-debug [slug]`)

| Symptom | Likely cause | Action |
|---------|--------------|--------|
| static fail `osc_process` / `p[]` | fiction API copied | rewrite to mkII `osc.h` |
| link / ROM | tables too large | shrink tables, `-Os` |
| silent | empty `process()` | implement DSP |
| zipper | param steps | smooth in `setParameter` |
| alias | naive saw | band-limited table |

Minimal diff; re-run eval. Log root cause in `.agent/debug.md`.
