---
description: logue mkII — fix build or DSP failures after eval
---

# /logue-mkii-debug [slug]

**Skill:** `.agents/skills/logue-mkii-debug/SKILL.md`

**Inputs:** `.agent/eval.log`, build stderr, symptom (silent, alias, zipper, link error)

**Process:**

1. Classify: static API / compile / ROM / audio quality
2. Fix minimal diff in `osc.h` / `unit.cc` / `header.c`
3. Re-run `./tools/osc-eval-mkii.sh <slug>`

**Stop.** `Next: /logue-mkii-eval <slug>` when fix applied
