# SOP-002 — Add or change a profit agent

## Purpose

New revenue-facing agent appears **repo-wide** with skills, PM ADD, automation, and CI — not buried under `disklordz/agents/` only.

## Procedure

1. **Spec** — Add entry to `disklordz/agents/profit/_specs.json` (id, title, description, profit_lever, tools, mcp, automate, soul).
2. **Scaffold trees** — `python3 disklordz/agents/profit/scaffold_agents.py --publish-skills`
3. **Workflow map** — In `disklordz/agents/workflows/scaffold_workflows.py`, add:
   - `ANNOUNCE[id]` — first-person PM ADD blurb
   - `WORKFLOWS[id]` — platform, trigger, workflow_file, steps
   - `ACTIVATION[id]` — e.g. `ci_scheduled`, `runtime_api`, `manual_stub`
4. **Regenerate fleet** — `./scripts/sync-disklordz-agent-fleet.sh`
5. **Verify** — `python3 disklordz/agents/profit/scaffold_agents.py --check`
6. **Docs** — Update [DISKLORDZ_PROFIT_AGENTS.md](../../DISKLORDZ_PROFIT_AGENTS.md) table if fleet segment changes.
7. **PR** — Include sample: which CI or API path executes this agent.

## Checklist

- [ ] `.github/skills/disklordz-<id>/SKILL.md` exists
- [ ] `disklordz/agents/profit/<id>/` (8 files)
- [ ] `disklordz/agents/workflows/agents/<id>/automation.yaml`
- [ ] Listed in `DISKLORDZ_AGENTS.md` roster block (generated)
- [ ] Listed in `.github/agents/fleet.json` (generated)

## Template

[PM ADD announcement draft](../templates/TEMPLATE-pm-add-agent-entry.md) → then encode in `ANNOUNCE` in code.
