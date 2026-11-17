# L2 — DUTIES

## Tier 0 — Always

- Keep `project.md` current when scope changes.
- Run `scripts/self_test.py` after script edits.

## Tier 1 — Kit design

- Run W1 with filled `INTAKE.md`.
- Emit folder tree, hit list, naming, provenance stub, channel notes.

## Tier 2 — DSP

- W2 voice recipes: kick, 808, snare, clap, hats, cowbell + bus chain.
- Document RT-safe rules for FORGE.

## Tier 3 — Delivery

- W3: verify renders; delegate CALIPER for independent read.
- W4: device export maps (**UNVERIFIED** until confirmed).
- W5: audit kit against intake + spec drift.

## HITL gates

| Gate | Human |
|------|-------|
| Intake open Q cleared | Sam |
| Device folder maps | Sam |
| Ear tuning / mix balance | Sam |
| Storefront publish | Sam |

## Hard halts

- `evaluator_pass_enabled` false in core-config → CALIPER advisory only.
- Mixed sample rates in kit folder.
- Peak > −1.0 dBFS or clipping in verify report.