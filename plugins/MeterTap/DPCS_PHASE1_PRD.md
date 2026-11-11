# MeterTap — DPCS Phase 1 index

**Status:** Phase 1 complete · **Next:** Phase 2 Contract (headers only, compile-checked)

Phase 1 delivers **behaviour only** per [docs/DPCS.md](../../docs/DPCS.md) — no `vst/MeterTap/`, no headers, no CMake.

## Artifacts

| File | Role |
|------|------|
| [.ideas/creative-brief.md](.ideas/creative-brief.md) | PRD: audio + UI behaviour |
| [.ideas/parameter-spec.md](.ideas/parameter-spec.md) | APVTS parameter contract |
| [DPCS_PHASE0_CONSTITUTION.md](DPCS_PHASE0_CONSTITUTION.md) | Phase 0 scope |
| [status.json](status.json) | APC / DPCS tracker |

## Verification (Phase 1 done)

```bash
bash plugins/MeterTap/scripts/verify-phase1.sh
```

Exit **0** iff all Phase 1 markdown artifacts exist and `status.json` reports `dpcs_phase >= 1`.

## Phase 2 entry criteria

- Operator or `approve_architecture` (factory) satisfied.
- Run `/apc-plan MeterTap` or manual `architecture.md` stub (optional before headers).
- Phase 2 allowlist: header stubs only under future `vst/MeterTap/Source/` + factory-lib `PeakRmsMeter.h` contract sketch.
