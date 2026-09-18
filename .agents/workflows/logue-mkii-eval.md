---
description: logue mkII — static + build evaluation
---

# /logue-mkii-eval [slug]

**Skill:** `.agents/skills/logue-mkii-eval/SKILL.md`

**Prerequisites:** `impl` complete (`PORT_COMPLETE` or working `process()`)

**Actions:**

```bash
./tools/osc-eval-mkii.sh <slug> --static-only   # always
./tools/osc-eval-mkii.sh <slug>                 # if LOGUE_SDK + gcc
```

Record exit code and size output in `.agent/eval.log`.

Update `status.json` (`current_phase`: `eval`, `eval_passed`: true/false).

**Stop.** On pass: `Next: /logue-mkii-ship <slug>`. On fail: `/logue-mkii-debug <slug>`
