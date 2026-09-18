---
name: logue-mkii-eval
description: Run osc-eval-mkii.sh (/logue-mkii-eval).
---

# Eval (`/logue-mkii-eval [slug]`)

```bash
./tools/osc-eval-mkii.sh <slug> --static-only 2>&1 | tee src/mkii/oscillators/<slug>/.agent/eval.log
./tools/osc-eval-mkii.sh <slug> 2>&1 | tee -a src/mkii/oscillators/<slug>/.agent/eval.log
```

Interpret exit codes: 0 pass, 1 static fail, 2 build skipped, 3 build fail.

Update `status.json`:

```json
"current_phase": "eval",
"eval_passed": true,
"phases_completed": ["...", "eval"]
```

If fail, list concrete fixes; suggest `/logue-mkii-debug`.
