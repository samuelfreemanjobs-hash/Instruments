# VST Plugin Factory — Slack integration

Slack is the **human notification layer** for the Plugin Factory agent (`cursor-vst-plugin-factory`). It does not run the agent; it tells you when to start a Cloud Agent or triage CI.

## Channels

| Channel | Slack ID | Purpose |
|---------|----------|---------|
| **#agent-team** | `C0C3M2NDK7Z` | Roster, sync meetings, workflow decisions — [SLACK_AGENT_TEAM.md](SLACK_AGENT_TEAM.md) |
| **#disklordz-dev** | `C0C1X6S61B9` | **Webhook target** for factory inbox + schedule (engineering noise) |
| **#disklordz-ci** | `C0C1X6RMGTZ` | General CI (`slack-ci`) |

Point **Incoming Webhooks** for factory automation at **#disklordz-dev**; use **#agent-team** for human coordination.

## One-time setup

```bash
./scripts/setup-disklordz-integrations.sh slack-vst-factory \
  --webhook-url 'https://hooks.slack.com/services/...' \
  --mention-user-id U0XXXXXXXX   # optional @you on inbox handoffs
```

This sets GitHub secrets:

| Secret | Purpose |
|--------|---------|
| `SLACK_WEBHOOK_VST_FACTORY_URL` | Posts for factory inbox + schedule |
| `SLACK_MENTION_VST_FACTORY_USER_ID` | Optional ping on new handoffs |
| (fallback) `SLACK_WEBHOOK_URL` | Used if factory-specific secret unset |

## What posts automatically

| Event | Workflow | Message |
|-------|----------|---------|
| `disklordz/vst-factory/inbox/HO-*.json` pushed to **main** | [vst-factory-inbox-slack.yml](../.github/workflows/vst-factory-inbox-slack.yml) | WO id, title, branch, link to agent doc |
| **VST Plugin Factory schedule** completes | [vst-factory-schedule-slack.yml](../.github/workflows/vst-factory-schedule-slack.yml) | Pass/fail + link to run (Mon/Wed/Fri jobs) |

Related (separate webhooks):

- **#disklordz-ci** — general CI via `slack-ci` ([ci-slack-notify.yml](../.github/workflows/ci-slack-notify.yml))
- **Antigravity inbox** — `slack-antigravity` ([antigravity-inbox-slack.yml](../.github/workflows/antigravity-inbox-slack.yml))

## Manual tests

```bash
gh workflow run vst-factory-inbox-slack.yml \
  -f handoff_path=disklordz/vst-factory/inbox/HO-test.json

gh workflow run vst-factory-schedule-slack.yml -f conclusion=success
```

## When Slack fires, what you do

1. **Inbox handoff** — Open Cursor → Cloud Agent → repo **Instruments**, branch from the message, prompt from [VST_PLUGIN_FACTORY_AGENT.md](VST_PLUGIN_FACTORY_AGENT.md).
2. **Schedule failure** — Run Mon **health triage** or Wed **Night Circuit QA** automation ([VST_PLUGIN_FACTORY_SCHEDULE.md](VST_PLUGIN_FACTORY_SCHEDULE.md)); open a WO if needed.
3. **Schedule success** — No action unless you are on weekly doc-sync duty (Fri).

## Slack MCP (interactive)

In Cursor chat you can use the **Slack** MCP to search threads or post updates; **write** actions (send message) need your approval per security policy. Use MCP for ad-hoc status, not for replacing the GitHub webhooks above.

## Related

- [VST_PLUGIN_FACTORY_SCHEDULE.md](VST_PLUGIN_FACTORY_SCHEDULE.md)
- [disklordz/automation/VST_PLUGIN_FACTORY.md](../disklordz/automation/VST_PLUGIN_FACTORY.md)
