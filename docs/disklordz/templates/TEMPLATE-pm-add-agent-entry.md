# PM ADD — draft before adding to scaffold_workflows.py

Add to `ANNOUNCE` and `WORKFLOWS` in `disklordz/agents/workflows/scaffold_workflows.py`, then run `./scripts/sync-disklordz-agent-fleet.sh`.

## Announcement (first person)

I'm **[Title]** — [one sentence: how I help Disklordz revenue/ops].

## Automation

| Field | Value |
|-------|--------|
| platform | github \| inngest \| api \| n8n \| manual |
| trigger | e.g. daily, POST /api/…, workflow_dispatch |
| workflow_file | path in repo |
| steps | 1. … 2. … |

## Activation code

`ci_scheduled` | `runtime_api` | `manual_stub` | … (see `ACTIVATION` in scaffold_workflows.py)

## Spec snippet (_specs.json)

```json
{
  "id": "my-agent-id",
  "title": "My Agent",
  "description": "…",
  "profit_lever": "…",
  "upstream_repo": "https://github.com/…",
  "model": "inherit",
  "permission_mode": "plan",
  "max_turns": 40,
  "tools": ["Read", "Grep", "Bash"],
  "mcp": [],
  "automate": "…",
  "soul": ["…", "…"]
}
```
