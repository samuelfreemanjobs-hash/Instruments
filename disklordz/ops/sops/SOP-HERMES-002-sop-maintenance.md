# SOP-HERMES-002 — SOP authoring and audit

| Field | Value |
|-------|--------|
| **Owner seat** | hermes-sop |
| **Consumer seats** | hermes-sop, hermes-lead, hermes-ops, all seats (read) |
| **Cadence** | weekly_or_on_gap |

## Purpose

Keep Disklordz **operating procedures** accurate, indexed, and aligned with elite seat skills and automation.

## Trigger

- New product, CI stage, deploy path, or integration.
- `hermes_tool sop audit` failure.
- Seat playbook learning that should become org-wide ([HERMES_AGENT_REPOS.md](../../../docs/HERMES_AGENT_REPOS.md)).
- User request for “OS” or “operations” improvement.

## Procedure

1. Run audit: `python3 disklordz/hermes/scripts/hermes_tool.py sop audit`.
2. For each gap (missing file, orphan SOP, seat without coverage):
   - Interview **owner seat** via Task (`hermes-<owner>:` confirm steps) or read their `hermes-elite-*/SKILL.md`.
   - Draft SOP from [templates/sop-procedure.md](../../hermes/templates/sop-procedure.md).
   - Add entry to [INDEX.json](INDEX.json) and [README.md](README.md).
3. Cross-link from owner skill (one line “Org SOP: SOP-…”) via PR — **hermes-sop** opens PR; owner approves accuracy.
4. Never duplicate entire elite skills in SOPs — SOP = procedure; skill = seat behaviour.
5. Record run: `hermes_tool.py agent record-run --seat hermes-sop --summary "…"`.

## Verification

- `sop audit` exits 0.
- Every `INDEX.json` `path` exists under `disklordz/ops/`.

## Escalation

Pricing, legal, production deploy approval → human Planner; document as “human gate” in SOP only.

## Related

- [docs/HERMES_SOP_OPERATIONS.md](../../../docs/HERMES_SOP_OPERATIONS.md)
- [disklordz/ops/ARCHITECTURE.md](../ARCHITECTURE.md)
