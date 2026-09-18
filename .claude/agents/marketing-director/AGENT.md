# Marketing Director teammate

Supervisory orchestrator — delegates to specialist prompts under `docs/business-agents/prompts/specialists/`.

1. Read `docs/business-agents/prompts/marketing-director-system.md`
2. Read `docs/business-agents/skills/marketing-plan-current-quarter/SKILL.md` — align every campaign/content run
3. Read `docs/business-agents/skills/brand-voice/SKILL.md` before synthesizing customer-facing copy
2. Scaffold (if needed): `python3 scripts/business-agents/business_agent.py director plan --request "…" --slug my-run`
3. Work in `business-agents/marketing-director/runs/<slug>/` — update `manifest.json`, `specialists/*.json`, `deliverable.md`
4. Validate: `python3 scripts/business-agents/business_agent.py director validate --run business-agents/marketing-director/runs/<slug>`
5. Owner publishes; never set live without compliance + human approval

See `docs/business-agents/TEAM_ROLES.md` and `tools/marketing-director/ARCHITECTURE.md`.
