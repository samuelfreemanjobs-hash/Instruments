# SOP-VF-00 — VST Factory master procedure

## Purpose

One page that tells anyone (human or agent) how a plugin goes from idea to shipped zip in this repo, which SOP covers each step, and what "done" means at each gate.

## The lifecycle

```text
INTAKE ─► DREAM ─► PLAN ─► DESIGN ─► CONTRACT ─► IMPLEMENT ─► VERIFY ─► CERTIFY ─► SHIP
 VF-07     VF-01    VF-04   VF-02      VF-05       VF-05        VF-06     VF-06      VF-08
           DPCS 0-1                    DPCS 2      DPCS 3       DPCS 4
                     gate: approve_architecture ─┘        gate: approve_gui ─┘   gate: approve_release ─┘
```

| Stage | SOP | Artifact (template) | Done when (exit 0) |
|-------|-----|---------------------|--------------------|
| Intake | [VF-07](SOP-VF-07-factory-operations.md) | factory project + `factory-spec/v2` | `POST /api/projects` → 200 |
| Dream | [VF-01](SOP-VF-01-intake-dream.md) | constitution, creative brief, parameter spec | `bash plugins/<Name>/scripts/verify-phase1.sh` |
| Plan | [VF-04](SOP-VF-04-plan.md) | architecture.md, apc-plan.md | `bash tools/apc-gate.sh <Name> design` |
| Design | [VF-02](SOP-VF-02-ui-design.md) | design-spec.md (theme roles) | `bash tools/apc-gate.sh <Name> impl` |
| Contract | [VF-05](SOP-VF-05-contract-implement.md) | headers only | cmake compile |
| Implement | [VF-05](SOP-VF-05-contract-implement.md) | one module per run | module tests |
| Verify | [VF-06](SOP-VF-06-verify-test.md) | verify-phase4.sh | `bash plugins/<Name>/scripts/verify-phase4.sh` |
| Certify | [VF-06](SOP-VF-06-verify-test.md) | factory certification | `POST /api/projects/{id}/certify` → passed |
| Ship | [VF-08](SOP-VF-08-release-ship.md) | release zip + notes | `bash tools/apc-gate.sh <Name> ship` + `tools/package-vst.sh <Name>` |

Supporting SOPs: [VF-03 theme](SOP-VF-03-design-system-theme.md) · [VF-09 debug](SOP-VF-09-debug-repair.md) · [VF-10 reusable module](SOP-VF-10-reusable-module.md) · [VF-11 agent delegation](SOP-VF-11-agent-delegation.md) · [VF-12 Windows operator](SOP-VF-12-windows-operator.md).

## Rules that apply at every stage

1. **One phase per turn** unless the operator says "full chain" or has approved autonomous work.
2. **DONE = a command exits 0.** "Looks fine" is not done. Record the command and its output in the PR.
3. **Specs are frozen once the next phase starts.** A spec bug found later becomes an *open issue* with a default, written back into the spec by the operator or with their ack.
4. **Reuse first.** Before writing DSP or UI, search `shared/dsp/`, `vst/factory-lib/`, and the factory catalog (`disklordz/vst-factory/modules/`).
5. **Every product doc starts from a template** (`bin/apc template`), so every product is reviewable the same way.
6. **Branch per phase or per agent**, draft PR, CI green before merge. Never push to `main` directly.
7. **Evidence tags** on decision surfaces (PR, report): `[executed]`, `[inspected]`, `[assumed]`.
8. **Memory:** recall lessons before each stage and record new verified ones after it (`scripts/agent_memory.py`, [SOP-019](SOP-019-agent-memory.md)). Inspect artefacts yourself; passing tests are not evidence that renders or packages are right.

## Roles

| Role | Does |
|------|------|
| Operator (owner) | Approves gates (architecture, GUI, release), answers open issues, merges |
| Factory service | Tracks state, enforces gates, posts to Slack, runs build/test/ship jobs |
| Agents (Cursor / Cloud) | Execute one SOP stage at a time inside an owned path set |

## Status

`plugins/<Name>/status.json` is the single source of phase truth. The factory reads it (`POST /api/projects/{id}/sync`); humans read it with `bin/apc status <Name>`.

## Related

- [docs/vst-lifecycle.md](../../vst-lifecycle.md) · [docs/DPCS.md](../../DPCS.md) · [docs/DISKLORDZ_VST_FACTORY_PLATFORM.md](../../DISKLORDZ_VST_FACTORY_PLATFORM.md)
- Templates index: [templates/apc/README.md](../../../templates/apc/README.md)
