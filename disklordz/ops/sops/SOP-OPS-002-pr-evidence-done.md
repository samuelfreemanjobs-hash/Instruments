# SOP-OPS-002 — PR title, evidence, and Done

| Field | Value |
|-------|--------|
| **Owner seat** | hermes-ops |
| **Consumer seats** | hermes-lead, hermes-qa, hermes-web |
| **Cadence** | every_pr |

## Purpose

Every PR proves its WO acceptance criteria with reproducible evidence.

## Procedure

1. Title contains `WO-…` when tracked ([ops validate-pr](#verification)).
2. Body lists: acceptance criteria, seats involved, **commands run**, artefact paths.
3. **Plugins:** `run_business.py --profile ci-verify` or subset; pluginval; GUI evidence for UI ([hermes-elite-qa](../../../.cursor/skills/hermes-elite-qa/SKILL.md)).
4. **SaaS:** `npm ci && npm run build && npm test`; browser walkthrough for UI.
5. **Docs-only:** link preview; no fake “tested” claims.
6. On merge: hermes-handoff updates HO/Airtable notes if applicable — human Done.

## Verification

CI green on GitHub **Build** (plugins) / web workflow when touched.

## Related

- [SOP-DEV-001](SOP-DEV-001-plugin-ci.md)
- [SOP-SAAS-001](SOP-SAAS-001-web-build-deploy.md)
- [docs/AGENTIC_PROJECT_STANDARDS.md](../../../docs/AGENTIC_PROJECT_STANDARDS.md)
