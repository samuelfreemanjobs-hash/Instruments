# Slack — agent team operations

**Home channel:** `#agent-team` (`C0C3M2NDK7Z`) — roster, meetings, workflow decisions.  
**Factory signals:** `#disklordz-dev` — inbox + schedule webhooks ([VST_PLUGIN_FACTORY_SLACK.md](VST_PLUGIN_FACTORY_SLACK.md)).

## Channel map

| Channel | ID | Use |
|---------|-----|-----|
| `#agent-team` | `C0C3M2NDK7Z` | Agent roster, sync meetings, automation decisions |
| `#disklordz-dev` | `C0C1X6S61B9` | VST factory + Antigravity inbox posts, engineering |
| `#disklordz-ci` | `C0C1X6RMGTZ` | Build / Nightly QA results |
| `#disklordz-product` | `C0C2G7S8K0U` | SKUs, Night Circuit M1/M2, roadmap |

## Webhooks (GitHub Actions → Slack)

```bash
./scripts/setup-disklordz-integrations.sh slack-vst-factory --webhook-url '...'  # → recommend #disklordz-dev
./scripts/setup-disklordz-integrations.sh slack-ci --webhook-url '...'           # → #disklordz-ci
./scripts/setup-disklordz-integrations.sh slack-antigravity --webhook-url '...'
```

## Weekly sync (human)

**Suggested:** Wednesday or Sunday **11:00 PM America/New_York** in `#agent-team` (async-friendly: thread replies if live call skipped).

Agenda template:

1. **Status** — Night Circuit M1, open PRs, CI red/green  
2. **Needs** — secrets, host testing (FL/MPC), missing Cursor Automations  
3. **Products** — M1 exit criteria, M2 queue  
4. **Schedule** — [VST_PLUGIN_FACTORY_SCHEDULE.md](VST_PLUGIN_FACTORY_SCHEDULE.md) UTC calendar  
5. **Per-agent workflows** — inbox → implement → QA → sound-design → merge  
6. **Roster gaps** — agents to add or retire  

## Agents (needed set)

See [AGENT_TEAM_ROSTER.md](AGENT_TEAM_ROSTER.md). Minimum for Night Circuit ship:

- `pm-agent`, `workflow-automation-agent`, `cursor-vst-plugin-factory`
- `night-circuit-qa`, `night-circuit-sound-design`
- Human: `factory-manager`, optional `business-planner` / `marketing` for SKU

## Automated work completion

GitHub emits **inbox handoffs** on UTC cron → **#disklordz-dev** Slack → Cloud Agent → PR + **outbox** done.

See [AUTOMATED_WORK_COMPLETION.md](AUTOMATED_WORK_COMPLETION.md) and `gh workflow run agent-scheduled-handoffs.yml`.

## Slack MCP (Cursor)

Use for meeting posts, threads, and search. GitHub webhooks remain source of truth for CI/inbox events.
