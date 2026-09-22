---
name: mpc-keygroup-program
description: "Plan and validate MPC Software keygroup programs via KeygroupSpec JSON. Use for MPC-ready multisample programs and drum keygroups."
---

# MPC Software keygroup program

## Phase 0 (planning)

MPC project formats are **proprietary**. Do not corrupt binary `.xpm` files without a validated exporter.

## Canonical intermediate: KeygroupSpec JSON

Fields (illustrative): `name`, `programType` (keygroup | drum), `zones[]` with `samplePath`, `rootKey`, `loKey`, `hiKey`, `loVel`, `hiVel`, `tuneCents`, `level`, `filter`, `envelope`, `chokeGroup`.

Schema file to add: `disklordz/sound-factory/schemas/keygroup-spec.json` (implementation WO).

## Export paths (choose per WO)

| Path | Agent |
|------|--------|
| **UI automation** | Bytebot / `computerUse` on Windows with MPC Software |
| **Project folder research** | Spike WO — parse MPC-exported text/XML if exposed |
| **Manual handoff** | README steps + KeygroupSpec printout for human |

## Inputs

- WAVs + `DISKLORDZ_*_MANIFEST` or SFZ map
- MPC neutral preset lane: `mpc-ready-808` in `prompt-params.ts`

## Confirmation

Any write into user's MPC project directory requires **interactive mode** user approval.
