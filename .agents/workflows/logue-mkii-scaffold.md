---
description: logue mkII — generate SDK v2 unit tree
---

# /logue-mkii-scaffold [slug]

**Skill:** `.agents/skills/logue-mkii-scaffold/SKILL.md`

**Prerequisites:** `design.md` exists (or user waived design for catalog port)

**Actions:**

- New from v1: `python3 tools/mkii/scaffold-mkii.py oscillators/<v1-folder>`
- Catalog refresh: `python3 tools/mkii/scaffold-mkii.py --all-bass` (careful: preserves `PORT_COMPLETE` osc.h)

**Outputs:** `src/mkii/oscillators/<slug>/` with stub `osc.h`, `PORTING.md`

Update `status.json` (`current_phase`: `scaffold`).

**Stop.** `Next: /logue-mkii-impl <slug>`
