---
description: logue mkII — build artifact and hardware load checklist
---

# /logue-mkii-ship [slug]

**Skill:** `.agents/skills/logue-mkii-ship/SKILL.md`

**Prerequisites:** `eval_passed` true (or user explicitly waives with documented risk)

**Actions:**

1. `./tools/build-mkii.sh <slug>`
2. Note path to `*.nts1mkiiunit`
3. Write `.agent/ship.md` — Kontrol Editor / loguecli steps per SDK README
4. Hardware smoke checklist (pitch sweep, knobs, sustain)

Update `status.json` (`current_phase`: `ship`, `shipped_at` ISO date).

**Stop.** Unit ready for user load on mkII hardware.
