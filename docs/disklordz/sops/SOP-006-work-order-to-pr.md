# SOP-006 — Work order → GitHub issue → PR

## Purpose

Keep **Disklordz OS** (Airtable) aligned with git history.

## Procedure

1. **Airtable** — Create or pick **Agent Work Order** with `work_order_id` (e.g. `WO-SAAS-012`).
2. **GitHub issue** — Title/body reference WO; link acceptance criteria from [DISKLORDZ_ILLUGEN_RESEARCH.md](../../DISKLORDZ_ILLUGEN_RESEARCH.md) when 007+.
3. **Branch** — `cursor/<short-description>-<suffix>` or team convention.
4. **PR title** — Include `WO-SAAS-NNN` when applicable.
5. **PR body** — [Template](../templates/TEMPLATE-pr-description-disklordz.md): goal, test evidence, env vars, out of scope.
6. **CI** — Website build + relevant agent workflows green.
7. **Merge** — Human or explicit user request; update Airtable Done.
8. **Handoff** — If phase complete, note in [ROADMAP.md](../../ROADMAP.md) milestone table.

## Automations

- **ship-velocity** / **airtable-wo-triage**: `.github/workflows/airtable-antigravity-handoff.yml` (repository_dispatch when configured).

## Template

[TEMPLATE-work-order-github.md](../templates/TEMPLATE-work-order-github.md)
