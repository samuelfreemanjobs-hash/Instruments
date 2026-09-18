---
description: logue mkII — algorithm and parameter mapping design
---

# /logue-mkii-design [slug]

**Skill:** `.agents/skills/logue-mkii-design/SKILL.md`

**Prerequisites:** `brief.md` exists

**Outputs:**

- `.agent/design.md` (pseudocode, anti-aliasing, host cutoff/res use, 0–1023 knob semantics)
- Update `status.json` (`current_phase`: `design`)

**No production DSP in `osc.h` yet** — design only unless fixing an existing port.

**Stop.** `Next: /logue-mkii-scaffold <slug>`
