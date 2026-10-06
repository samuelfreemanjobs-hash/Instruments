# SOP-009 — Repo-wide agent fleet governance

## Purpose

Prevent “hidden” agents (e.g. Workflow Automation only under `disklordz/agents/`). If it is not in **`DISKLORDZ_AGENTS.md`**, it does not exist for the company.

## Architecture

| Layer | Path |
|-------|------|
| Root index | `DISKLORDZ_AGENTS.md` |
| PM ADD | `disklordz/agents/workflows/PM_ADD.md` |
| Activation matrix | `docs/DISKLORDZ_AGENT_FLEET.md` |
| Machine registry | `.github/agents/fleet.json` |
| Cursor rule | `.cursor/rules/disklordz-agent-fleet.mdc` |

## Orchestration agents

- **workflow-automation** — scaffolds PM ADD + `automation.yaml`
- **pm-agent** — governance CI + fleet execute

## Procedure after any fleet change

```bash
./scripts/sync-disklordz-agent-fleet.sh
git add DISKLORDZ_AGENTS.md docs/DISKLORDZ_AGENT_FLEET.md .github/agents/fleet.json disklordz/agents/workflows/
git commit -m "Sync agent fleet docs"
```

CI `agent-fleet-governance.yml` fails if generated files drift from specs.

## Lesson (from fleet rollout)

Always update **root** `AGENTS.md` and `ARCHITECTURE.md` agent counts when adding orchestration agents — Cloud Agents read root first.
