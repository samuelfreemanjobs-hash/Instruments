---
name: apc-debug
description: APC Debug — build errors, silence, clicks.
---

# Debug phase (`/apc-debug [Name]`)

1. Read last build log / user symptom.
2. Check realtime rules (alloc in processBlock, missing smoothing).
3. Append short entry to `status.json` → `error_recovery.error_log`.
4. Fix minimal diff; re-run `/apc-test`.

Skill: `.cursor/skills/dsp-parameter-mapping/SKILL.md` for zipper noise.
