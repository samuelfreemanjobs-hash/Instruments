# Slack — Disklordz workspace wiring

Channel map (Slack workspace `revenuesystem-4o91279`):

| Channel | ID | Automation |
|---------|-----|------------|
| **#disklordz** | `C0C2AK80XCN` | **Audio Coach** daily nudge (`DISKLORDZ_COACH_WEBHOOK_URL`) |
| **#disklordz-ci** | `C0C1X6RMGTZ` | GitHub CI (`SLACK_WEBHOOK_URL`, `ci-slack-notify.yml`) |
| **#disklordz-dev** | `C0C1X6S61B9` | Antigravity handoffs (`SLACK_WEBHOOK_ANTIGRAVITY_URL`) |

Machine-readable: [`disklordz/integrations/slack-channels.json`](../disklordz/integrations/slack-channels.json)

## Automated (no webhook) — Cursor Cloud

**Active on this repo:** daily timer `disklordz-coach-slack-daily` posts coach output to **#disklordz** via Slack MCP (14:00 UTC).

No GitHub secret required. Runs when this Cloud Agent fires on schedule.

## Optional: GitHub Actions (runs if Cloud is offline)

Prefer **bot token** (one secret, any message shape):

1. [Slack API](https://api.slack.com/apps) → Create app → **OAuth** scope `chat:write` → Install to workspace → invite bot to **#disklordz**.
2. `gh secret set SLACK_BOT_TOKEN --repo samuelfreemanjobs-hash/Instruments`
3. Enable variable `AUDIO_COACH_NUDGES_ENABLED=true`
4. `gh workflow run audio-coach-daily-nudge.yml`

Fallback: incoming webhook → `DISKLORDZ_COACH_WEBHOOK_URL`.

## One-time: Coach → #disklordz (webhook path)

1. Slack → **Apps** → **Incoming Webhooks** → **Add to Slack** → channel **#disklordz**.
2. Copy the `https://hooks.slack.com/services/...` URL.
3. On your machine (needs `gh auth login`):

```bash
./scripts/setup-disklordz-integrations.sh slack-coach --webhook-url 'https://hooks.slack.com/services/...'
```

That sets:

- GitHub secret `DISKLORDZ_COACH_WEBHOOK_URL`
- Repo variable `AUDIO_COACH_NUDGES_ENABLED=true`

4. Test:

```bash
gh workflow run audio-coach-daily-nudge.yml -R samuelfreemanjobs-hash/Instruments
```

Or locally:

```bash
DISKLORDZ_COACH_WEBHOOK_URL='...' python3 disklordz/audio-command/scripts/coach_outreach.py --send
```

## CI → #disklordz-ci

```bash
./scripts/setup-disklordz-integrations.sh slack-ci --webhook-url '...'
```

## Cursor / agents

- **audio-business-copilot** — references this doc for outreach
- **plugin-factory-liaison** — may paste `PASTE_SLACK` for Factory status
- **Gemini Spark** — `PASTE_SLACK` blocks for you to forward
- **Cursor Cloud** with Slack MCP — `slack_send_message` to `C0C2AK80XCN` for ad-hoc updates

## Troubleshooting

| Symptom | Fix |
|---------|-----|
| No daily coach message | Secret missing or `AUDIO_COACH_NUDGES_ENABLED` not `true` |
| CI never posts | Run `slack-ci`; check `SLACK_WEBHOOK_URL` |
| Wrong channel | Webhooks are per-channel — create a new webhook for #disklordz |
