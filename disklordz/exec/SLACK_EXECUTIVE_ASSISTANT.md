# Slack Executive Assistant (EA)

Your **EA lives in Slack** as a **workflow + channel discipline**, backed by the **exec digest API**. No separate SaaS required for v1.

## Setup

### 1. Channel

**#disklordz-exec** — founder, PM, SAGE summaries, blockers only (not general dev chat).

Marketing stays `#disklordz-marketing-dept`; CI stays `#disklordz-ci`.

### 2. Daily digest (automated)

**Option A — Slack Workflow Builder (recommended)**

1. Slack → **Workflows** → Create → Schedule (weekdays 8am your TZ)  
2. Step: **HTTP request** GET  
   - URL: `https://YOUR_DOMAIN/api/exec/digest?secret=YOUR_CRON_SECRET`  
   - Or header `Authorization: Bearer YOUR_AUTOMATION_SECRET`  
3. Step: **Send message** to `#disklordz-exec`  
   - Body: use JSON field `slackText` from response (or paste whole JSON for debugging)

**Option B — GitHub Action cron**

Add workflow that curls digest and POSTs to `SLACK_WEBHOOK_ANTIGRAVITY_URL` with `slackText`.

**Option C — Zapier read**

Zapier MCP **read** only: schedule → GET digest URL → Slack post (confirm before enabling write zaps).

### 3. How you talk to the EA

| Message in #disklordz-exec | Routed to |
|----------------------------|-----------|
| `SAGE: status` | Chief of Staff prompt / digest |
| `SAGE: factory done` | CROW + HELM checklist |
| `SAGE: what's blocking launch?` | INTEGRATION_AUDIT P0 |
| `PM: schedule …` | PM Agent WO |

Until a Slack bot app exists, **you** or **Cursor with Slack MCP** posts digests; SAGE is the **playbook** for routing.

### 4. Secrets (PM)

| Secret | Purpose |
|--------|---------|
| `CRON_SECRET` or `AUTOMATION_SECRET` | Protect `/api/exec/digest` |
| `LAUNCH_PUBLIC_BASE_URL` | Correct links in emails/checkout |

### 5. Future: Slack app "Disklordz EA"

Dedicated bot with `/disklordz status` → same digest API. FORGE WO when message volume justifies it.

## API

```bash
curl -s -H "Authorization: Bearer $AUTOMATION_SECRET" \
  "https://YOUR_DOMAIN/api/exec/digest"
```

Returns `slackText` ready to paste or pipe to webhook.
