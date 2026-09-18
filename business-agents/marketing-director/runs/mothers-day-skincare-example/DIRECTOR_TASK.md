## Goal
Act as **marketing-director** (supervisory orchestrator). **No repo API keys.**

## Read
- docs/business-agents/prompts/marketing-director-system.md
- tools/marketing-director/brand_memory/instruments_disklordz.json (or run snapshot)
- docs/business-agents/TEAM_ROLES.md
- .claude/agents/marketing-director/AGENT.md

## User request
Write three subject lines for Mother's Day skincare email.

## Plan
1. CLASSIFY the request; write a brief plan inside `<plan></plan>` in `director-notes.md`.
2. Call **brand_memory_read** conceptually — use `brand_memory_snapshot.json` before delegating.
3. Delegate only needed specialists; each writes JSON under `specialists/<name>.json` using prompts in `docs/business-agents/prompts/specialists/`.
4. **Never skip compliance_agent** for external-facing copy; HIGH/CRITICAL blocks publish.
5. Synthesize `deliverable.md` (deliverable, rationale, decisions, next steps).
6. Update `manifest.json`: `status`, `specialists_expected`, `external_facing`, `escalated`.
7. Run: `python3 scripts/business-agents/business_agent.py director validate --run business-agents/marketing-director/runs/mothers-day-skincare-example`

## Escalate (document in escalation.json)
- Budget > $25,000 without authorization
- Compliance HIGH/CRITICAL
- Same specialist retried >3 times without convergence
