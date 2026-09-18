---
description: Continue mkII oscillator work from status.json
---

# /logue-mkii-resume [slug]

1. Load `status.json` and `docs/logue-mkii-workflow.md` phase order
2. Run the **next** incomplete phase workflow (do not skip eval before ship)

| `current_phase` | Next command |
|-----------------|--------------|
| (missing) | `/logue-mkii-brief` |
| brief | `/logue-mkii-design` |
| design | `/logue-mkii-scaffold` |
| scaffold | `/logue-mkii-impl` |
| impl | `/logue-mkii-eval` |
| eval (failed) | `/logue-mkii-debug` |
| eval (passed) | `/logue-mkii-ship` |
| ship | done — suggest hardware test |

**Stop** after executing one phase.
