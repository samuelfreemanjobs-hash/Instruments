# Factory agents → Slack

You should **hear from the label team** in Slack when the factory runs—not only CI bots.

## Channel setup

1. Create **#disklordz-factory** (or reuse **#disklordz-ci** for testing).
2. Add an **Incoming Webhook** for that channel.
3. Configure the secret:

```bash
./scripts/setup-disklordz-integrations.sh slack-factory --webhook-url 'https://hooks.slack.com/services/...'
```

This sets GitHub secret **`SLACK_FACTORY_WEBHOOK_URL`** and sends an **agent check-in** (all 11 personas).

Local dev:

```bash
export SLACK_FACTORY_WEBHOOK_URL='https://hooks.slack.com/services/...'
./scripts/factory-slack-checkin.sh
```

## When agents post

| Event | Who speaks in Slack |
|--------|---------------------|
| **Agent check-in** | Full roster (Orchestrator, Market Intel, A&R, Producer, …) |
| **Night shift complete** | Orchestrator + Market Intel + **A&R lineup** + Producer, Sample Miner, Content, Rights |
| **You approve an asset** | Content Producer + A&R confirmation |

Night shift: `POST /night-shift/run` or dashboard **Run night shift** (Slack on by default).

## GitHub schedule

Workflow [`.github/workflows/factory-agent-slack.yml`](../.github/workflows/factory-agent-slack.yml):

- Weekdays 12:00 UTC — agent check-in
- Manual: `gh workflow run factory-agent-slack.yml`

## API

| Endpoint | Purpose |
|----------|---------|
| `POST /factory/slack/checkin` | All agents introduce themselves |
| `GET /factory/slack/preview` | JSON payloads without sending |

Env: **`SLACK_FACTORY_WEBHOOK_URL`** (preferred) or **`SLACK_WEBHOOK_URL`**.

Optional: **`FACTORY_DASHBOARD_URL`** — link inserted in Orchestrator messages.

## Marketing research honesty

A&R and Market Intel messages label **heuristic briefings** until live intel feeds write `DL-OPP-*` records. The Slack thread still fires so you can react and steer the next batch.
