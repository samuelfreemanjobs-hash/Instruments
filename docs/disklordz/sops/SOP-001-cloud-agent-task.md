# SOP-001 — Run a Disklordz Cloud Agent task

## Purpose

Consistent execution for any Disklordz change (SaaS, RAG, agents, integrations) with evidence and no surprise prod deploys.

## Prerequisites

- Read [ARCHITECTURE.md](../../../ARCHITECTURE.md), [AGENTS.md](../../../AGENTS.md), [DISKLORDZ_AGENTS.md](../../../DISKLORDZ_AGENTS.md)
- Branch: `cursor/<description>-<suffix>` when Cloud requires it

## Procedure

1. **Scope** — One vertical slice; list out-of-scope in the prompt ([template](../templates/TEMPLATE-cloud-agent-prompt.md)).
2. **Product doc** — Open the product `ARCHITECTURE.md` (e.g. `disklordz/website/ARCHITECTURE.md`).
3. **Implement** — Match existing patterns; minimal diff.
4. **Test** — Run commands from `AGENTS.md`:
   - Web: `cd disklordz/website && npm ci && npm run build`
   - SaaS smoke: `DISKLORDZ_URL=… bash disklordz/website/scripts/verify-go-live.sh`
   - Plugin: `python3 vst-testing-ops/run_business.py --profile ci` if touching C++
5. **Artifacts** — Screenshot/video/logs for UI or non-trivial behavior.
6. **Docs** — Update `ARCHITECTURE.md` if structure changed; `.env.example` for new env **names** only.
7. **Git** — Commit, push, **draft PR** unless user asked to merge.
8. **Fleet** — If `_specs.json` or workflows changed: `./scripts/sync-disklordz-agent-fleet.sh`.

## Success criteria

- [ ] Success state proven (not “compiles only”)
- [ ] No secrets in git
- [ ] PR links WO id when applicable (`WO-SAAS-NNN`)

## Escalation

- Prod deploy / Supabase prod / Stripe writes → explicit human confirmation (META R10).
