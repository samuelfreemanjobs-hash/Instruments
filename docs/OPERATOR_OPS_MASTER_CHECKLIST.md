# Operator ops master checklist

One ordered path to turn on Slack CI, optional Slack for Antigravity, Disklordz go-live automation, the Airtable → Antigravity handoff, and the Windows MPC download inbox — plus what stays blocked.

Tags:

- **HUMAN** — you click or paste in Slack / GitHub / Vercel / Supabase / Stripe / Airtable UI, or run a command on your own machine. Agents must not do these ([CLAUDE.md](../CLAUDE.md) R10).
- **AGENT/CI** — already automated in a GitHub Actions workflow or repo script; you only trigger it (or it runs on its own).

**Secret values never go in git, issues, PRs, or chat.** This doc lists secret *names* only. When a step asks you to enter a value, it goes straight into GitHub/Vercel/Slack UI or a hidden terminal prompt.

Detail docs: [DISKLORDZ_GO_LIVE_SECRETS.md](DISKLORDZ_GO_LIVE_SECRETS.md) · [DISKLORDZ_GO_LIVE.md](DISKLORDZ_GO_LIVE.md) · [REPO_AUTOMATION.md](REPO_AUTOMATION.md) · [disklordz/automation/README.md](../disklordz/automation/README.md) · [disklordz/daw-inbox/README.md](../disklordz/daw-inbox/README.md) · [OPERATOR_AUTOPILOT.md](OPERATOR_AUTOPILOT.md)

---

## Your next 5 HUMAN actions

1. **Slack webhook for CI** — create an Incoming Webhook for **#disklordz-ci** (A1).
2. **Store it as `SLACK_WEBHOOK_URL`** — install `jq`, then run `slack-ci` with the hidden prompt (A2–A3), and fire the manual test (A4).
3. **Create the `agent-run` and `needs-secrets` labels** in GitHub (0.2) — the Agent run issue form applies `agent-run`, but the label does not exist yet, so GitHub drops it.
4. **Two Supabase secrets minimum:** `SUPABASE_ACCESS_TOKEN` + `SUPABASE_PROJECT_REF` (plus `SUPABASE_DB_PASSWORD`), then run **Disklordz Supabase migrations** with **dry_run** (B5–B7).
5. **Windows MPC:** `cd disklordz\daw-inbox` → `npm ci` → `npm run once` (E1).

Status when this doc was written: the repo had **zero** GitHub Actions secrets (`gh secret list` empty), so every secret-gated workflow skips or fails until Phases A–C are done.

---

## Phase 0 — Prerequisites (one time, ~5 min)

| # | Tag | Step |
|---|-----|------|
| 0.1 | HUMAN | Windows: install **Git for Windows** (Git Bash), **GitHub CLI** (`gh auth login`), **Node.js LTS** (18+), and **jq**: `winget install jqlang.jq` — open a new Git Bash and check `jq --version`. Without `jq`, the Slack setup script sets the secret but then fails at the test message. |
| 0.2 | HUMAN | GitHub → Instruments → **Issues → Labels → New label**: `agent-run` and `needs-secrets`. (Or: `gh label create agent-run --repo samuelfreemanjobs-hash/Instruments`; same for `needs-secrets`.) |
| 0.3 | HUMAN | Check which secret **names** exist at any time (values are never shown): `gh secret list --repo samuelfreemanjobs-hash/Instruments` |

---

## Phase A — Slack

### A.1 CI notifications → #disklordz-ci

| # | Tag | Step |
|---|-----|------|
| A1 | HUMAN | Slack → [api.slack.com/apps](https://api.slack.com/apps) → your app (or **Create New App → From scratch**) → **Incoming Webhooks** → On → **Add New Webhook to Workspace** → pick **#disklordz-ci** → copy the URL. Do not paste it into chat or any file. |
| A2 | HUMAN | Git Bash at repo root (`gh` must be logged in). Enter the URL at a hidden prompt so it stays out of shell history: |
| | | `read -rsp "Slack webhook URL: " SLACK_WEBHOOK_URL; echo; export SLACK_WEBHOOK_URL` |
| A3 | HUMAN → AGENT/CI | `./scripts/setup-disklordz-integrations.sh slack-ci` then `unset SLACK_WEBHOOK_URL`. The script sets GitHub secret **`SLACK_WEBHOOK_URL`** and posts a test message to #disklordz-ci. UI alternative: GitHub → Settings → Secrets and variables → Actions → **New repository secret** `SLACK_WEBHOOK_URL` (then use `--skip-secret` only if you want the script's test message). |
| A4 | AGENT/CI (you trigger) | Test the workflow: `gh workflow run ci-slack-notify.yml --repo samuelfreemanjobs-hash/Instruments -f conclusion=success` (or Actions → **CI Slack notify** → Run workflow). Expect a "Disklordz CI — Manual test" message. |
| A5 | AGENT/CI | From now on, **CI Slack notify** (`ci-slack-notify.yml`) posts automatically whenever **Build**, **Build MyFirstPlugin**, or **Nightly QA** completes. Without the secret it skips quietly (that is why past runs show green with no message). |

Known: **Build** (`cmake`) and `windows-vst3` currently fail on every PR because CI cannot check out the pinned `clap_juce_extensions` commit — expect failure posts until that is fixed (separate task).

### A.2 Optional — Antigravity inbox → #disklordz-dev

| # | Tag | Step |
|---|-----|------|
| A6 | HUMAN | Create a second Incoming Webhook for **#disklordz-dev** (same app, A1). |
| A7 | HUMAN | Your Slack member id (not secret): Slack → your profile → **⋮** → **Copy member ID** (`U…`). Needs workspace setting "Show member IDs". |
| A8 | HUMAN → AGENT/CI | Git Bash: `read -rsp "Antigravity webhook URL: " SLACK_WEBHOOK_ANTIGRAVITY_URL; echo; export SLACK_WEBHOOK_ANTIGRAVITY_URL` → `export SLACK_MENTION_USER_ID=U0123456789` → `./scripts/setup-disklordz-integrations.sh slack-antigravity` → `unset SLACK_WEBHOOK_ANTIGRAVITY_URL`. Sets **`SLACK_WEBHOOK_ANTIGRAVITY_URL`** and **`SLACK_MENTION_USER_ID`**, posts a test message. |
| A9 | AGENT/CI | **Antigravity inbox Slack** (`antigravity-inbox-slack.yml`) posts when a new `disklordz/antigravity/inbox/HO-*.json` lands on `main`. Falls back to `SLACK_WEBHOOK_URL` if the Antigravity secret is unset. |
| A10 | AGENT/CI (you trigger) | Manual workflow test needs a **real** handoff file: the inbox is empty today, so the default input `HO-00000000-0000.json` fails with "File not found". Test after C4 creates one: `gh workflow run antigravity-inbox-slack.yml --repo samuelfreemanjobs-hash/Instruments -f handoff_path=disklordz/antigravity/inbox/<HO-file>.json`. Until then, A8's test message proves the webhook. |

---

## Phase B — Go-live (Vercel, Supabase, Stripe test mode)

### B.1 Dashboards (each produces secret values)

| # | Tag | Step | Produces (secret names) |
|---|-----|------|-------------------------|
| B1 | HUMAN | **Supabase** → project → Settings → API; Settings → Database; Account → Access tokens. | `NEXT_PUBLIC_SUPABASE_URL`, `NEXT_PUBLIC_SUPABASE_ANON_KEY`, `SUPABASE_SERVICE_ROLE_KEY`, `SUPABASE_PROJECT_REF` (URL subdomain), `SUPABASE_DB_PASSWORD`, `SUPABASE_ACCESS_TOKEN` |
| B2 | HUMAN | **Vercel** → Add New Project → import `Instruments` → **Root Directory = `disklordz/website`** → Deploy. First build needs no env vars; auth/billing pages work only after B8. | Production URL → `DISKLORDZ_URL` (no trailing slash) |
| B3 | HUMAN | Vercel → Project → Settings → General → **Project ID**; Account → **Tokens**; Team ID only for team projects. Settings → Git → **Deploy Hooks** → hook for `main`. | `VERCEL_PROJECT_ID`, `VERCEL_TOKEN`, (`VERCEL_TEAM_ID`), `VERCEL_DEPLOY_HOOK_URL` |
| B4 | HUMAN | Supabase → Authentication → URL configuration: Site URL = `DISKLORDZ_URL`; Redirect URL = `DISKLORDZ_URL/auth/callback`. | — |
| B5 | HUMAN | **Stripe, test mode** → Products → Pro with **recurring** price; Developers → API keys (`sk_test_…`); Developers → Webhooks → Add endpoint `DISKLORDZ_URL/api/stripe/webhook`, events `checkout.session.completed`, `customer.subscription.updated`, `customer.subscription.deleted` → reveal **Signing secret**. | `STRIPE_PRO_PRICE_ID`, `STRIPE_SECRET_KEY`, `STRIPE_WEBHOOK_SECRET` |

The workflow option **Register Stripe webhook** can create the endpoint instead, but it deliberately does **not** print the signing secret (public repo → public Actions logs). Reveal it in the Stripe Dashboard either way.

### B.2 GitHub Actions secrets (names only)

| # | Tag | Step |
|---|-----|------|
| B6 | HUMAN | GitHub → Settings → Secrets and variables → Actions → **New repository secret** for each name below. Terminal alternative you run yourself: `gh secret set NAME --repo samuelfreemanjobs-hash/Instruments` (prompts; never use `--body` with a real value). |

| Group | Secret names | Needed by |
|-------|--------------|-----------|
| Go-live preflight (required) | `SUPABASE_ACCESS_TOKEN`, `SUPABASE_PROJECT_REF`, `DISKLORDZ_URL`, `VERCEL_DEPLOY_HOOK_URL` | Disklordz go-live |
| Migrations in CI | `SUPABASE_DB_PASSWORD` | Supabase migrations, go-live, Activate integrations |
| Vercel env sync | `VERCEL_TOKEN`, `VERCEL_PROJECT_ID`, `VERCEL_TEAM_ID` (team only) | go-live (`sync_vercel_env`), Activate integrations |
| App runtime (synced to Vercel) | `NEXT_PUBLIC_SUPABASE_URL`, `NEXT_PUBLIC_SUPABASE_ANON_KEY`, `SUPABASE_SERVICE_ROLE_KEY`, `STRIPE_SECRET_KEY`, `STRIPE_WEBHOOK_SECRET`, `STRIPE_PRO_PRICE_ID`, `SAAS_DAILY_GEN_LIMIT` (optional, default 20) | go-live, Activate integrations |
| Integrations (optional) | `OPENAI_API_KEY`, `INNGEST_EVENT_KEY`, `INNGEST_SIGNING_KEY`, `DISKLORDZ_ENGINE`, `DISKLORDZ_AUDIOCRAFT_ENGINE_URL`, `DISKLORDZ_STABLE_AUDIO_ENGINE_URL` | Activate integrations |

### B.3 When to run which workflow

| # | Tag | When | Step |
|---|-----|------|------|
| B7 | AGENT/CI (you trigger) | As soon as the Supabase secrets exist | Actions → **Disklordz Supabase migrations** → **dry_run = true**. Read-only; proves token, project ref and DB password. |
| B8 | AGENT/CI (you trigger) — **production** | Only after **every** "required" and "app runtime" secret is set and B7 is green | Actions → **Disklordz go-live** → `sync_vercel_env = true`, `register_stripe_webhook = false`, rest default. Applies all migrations → upserts Vercel env (production + preview) → deploy hook → smoke test. This writes to the production DB and Vercel env: the R10 gate, which is why you start it. |
| B9 | AGENT/CI (you trigger) | B8 smoke failed only because the deploy was still building (it waits 45 s) | Re-run **Disklordz go-live** with `skip_migrations` + `skip_deploy` = true, `sync_vercel_env` = false → smoke only. |
| B10 | AGENT/CI (you trigger) | After changing any secret | Re-run **Disklordz go-live** with `sync_vercel_env = true`, `skip_migrations = true`. Vercel env changes apply only after the redeploy it triggers. |
| B11 | AGENT/CI (you trigger) | Optional, after B8 is green **and** you have set the integration secrets | Actions → **Activate Disklordz integrations**. Runs migrations again, RAG embed (skips without `OPENAI_API_KEY`), builds, syncs core + integration env to Vercel, triggers a deploy, verifies `/api/integrations/status`. Not needed for a working site. |

### B.4 Verify

| # | Tag | Step |
|---|-----|------|
| B12 | HUMAN | Remote smoke from your machine (Git Bash): `cd disklordz/website && DISKLORDZ_URL=https://your-app.vercel.app bash scripts/verify-go-live.sh` (PowerShell: `$env:DISKLORDZ_URL="https://your-app.vercel.app"; bash scripts/verify-go-live.sh`). Checks `/`, one-shot + loop generation, sample preview. |
| B13 | HUMAN | Browser: guest kit download · magic-link sign-in → kit on `/account` · Pro checkout with test card `4242 4242 4242 4242` → Stripe webhook delivery `200`. Full list: [DISKLORDZ_GO_LIVE.md §6](DISKLORDZ_GO_LIVE.md#6-production-smoke-test-manual). |
| B14 | HUMAN (later) | Live mode: repeat B5 in Stripe **live** mode (new keys, price, webhook — different signing secret), replace the three `STRIPE_*` secrets, then B10. |

---

## Phase C — Airtable → Antigravity handoff

| # | Tag | Step |
|---|-----|------|
| C1 | HUMAN | Airtable base with table **`Agent Work Orders`** and fields `work_order_id`, `title`, `acceptance_criteria`, `owner_agent`, `status`; optional `antigravity_handoff_id` (single line text). Seed rows: [`disklordz/airtable/seed/hise-sketch-lane-2026.json`](../disklordz/airtable/seed/hise-sketch-lane-2026.json). Different table name → also set secret `AIRTABLE_TABLE_WORK_ORDERS`. |
| C2 | HUMAN | Airtable → **Builder hub → Personal access tokens** → create token with `data.records:read` + `data.records:write` (write is used to patch `antigravity_handoff_id`), access limited to this base. Base id = `app…` part of the base URL. |
| C3 | HUMAN | GitHub secrets **`AIRTABLE_API_KEY`** (the token) and **`AIRTABLE_BASE_ID`**. |
| C4 | AGENT/CI (you trigger) | Actions → **Airtable → Antigravity handoff** → Run workflow → `work_order_id = WO-2026-HISE-001`, `branch = main`. Or `gh workflow run airtable-antigravity-handoff.yml --repo samuelfreemanjobs-hash/Instruments -f work_order_id=WO-2026-HISE-001`. **Note:** it commits `disklordz/antigravity/inbox/HO-*.json` directly to the chosen branch as `github-actions[bot]`. On `main` that push also triggers A9. |
| C5 | HUMAN | Airtable automation (trigger: `status` = `Ready for Antigravity`; filter `owner_agent` = `antigravity-hise` or title contains `[Plugin][HISE]`) → action **Run script** / Zapier webhook that sends the `repository_dispatch` below. Needs a GitHub fine-grained PAT (Instruments only, **Contents: Read and write**) kept in Airtable/Zapier credential storage — never in a base field or this repo. |

```http
POST https://api.github.com/repos/samuelfreemanjobs-hash/Instruments/dispatches
Authorization: Bearer <PAT — stored in Airtable/Zapier, not here>
Accept: application/vnd.github+json

{ "event_type": "airtable-antigravity-handoff",
  "client_payload": { "work_order_id": "WO-2026-HISE-001", "branch": "main" } }
```

| # | Tag | Step |
|---|-----|------|
| C6 | HUMAN | Antigravity Windows PC (once): `.\scripts\antigravity-bridge\install-antigravity-workspace.ps1`; after each `git pull`: `.\scripts\antigravity-bridge\sync-handoff.ps1`. |

---

## Phase D — Stub agents

| # | Tag | Step |
|---|-----|------|
| D1 | AGENT/CI | `bash disklordz/agents/workflows/scripts/stub-agent-env-check.sh` regenerates [STUB_AGENT_BLOCKERS.md](../disklordz/agents/workflows/STUB_AGENT_BLOCKERS.md). Its "This shell" column only sees local env vars — use `gh secret list` (0.3) for what CI has. |
| D2 | HUMAN | Unblock one area at a time using the table below. |

| Agent area | Needs | Unblocked by |
|------------|-------|--------------|
| integration-health / ship-velocity (Slack CI) | `SLACK_WEBHOOK_URL` | Phase A |
| billing-ops, analytics-interpreter | `STRIPE_SECRET_KEY` (+ Supabase read) + n8n import | B5–B6, then n8n |
| churn-winback | `SUPABASE_SERVICE_ROLE_KEY` + email provider in n8n | B1, B6, then n8n |
| airtable-wo-triage / Antigravity handoff | `AIRTABLE_API_KEY`, `AIRTABLE_BASE_ID` | Phase C |
| prompt-coach (pgvector RAG) | `OPENAI_API_KEY` + Supabase | B6 integrations row, B11 |
| async-generation / onboarding-concierge | `INNGEST_EVENT_KEY`, `INNGEST_SIGNING_KEY` | B6 integrations row, B11 |
| factory-batch-gpu | Modal / GPU engine URL | Optional; `DISKLORDZ_*_ENGINE_URL` |
| marketing-glue, billing-ops, churn-winback, analytics | n8n running + workflows imported | HUMAN: `docker compose -f disklordz/integrations/docker-compose.optional.yml --profile n8n up -d`, import `disklordz/agents/workflows/n8n/*.json`, set credentials in n8n UI |
| desktop-ops | Bytebot (local Docker) | [BYTEBOT_SETUP.md](BYTEBOT_SETUP.md) — external |

---

## Phase E — Windows MPC PC (kit downloads → inbox)

| # | Tag | Step |
|---|-----|------|
| E1 | HUMAN | PowerShell: `cd C:\path\to\Instruments\disklordz\daw-inbox` → `npm ci` → `npm run once`. Output shows `download: %USERPROFILE%\Downloads`, `inbox: %USERPROFILE%\Music\Disklordz\Inbox`, then `--once complete`. |
| E2 | HUMAN (optional) | Custom folders: `%USERPROFILE%\.config\disklordz\inbox.json` — full paths (no `%USERPROFILE%`), doubled backslashes: |

```json
{
  "downloadDir": "C:\\Users\\YOU\\Downloads",
  "inboxDir": "C:\\Users\\YOU\\Music\\Disklordz\\Inbox"
}
```

| # | Tag | Step |
|---|-----|------|
| E3 | HUMAN (optional) | Start at login — **Administrator** PowerShell, one line (find Node with `where.exe node`; replace both paths): |

```powershell
Register-ScheduledTask -TaskName "Disklordz DAW Inbox" -Action (New-ScheduledTaskAction -Execute "C:\Program Files\nodejs\node.exe" -Argument '"C:\path\to\Instruments\disklordz\daw-inbox\bin\watch.mjs"') -Trigger (New-ScheduledTaskTrigger -AtLogOn -User $env:USERNAME)
```

| # | Tag | Step |
|---|-----|------|
| E4 | HUMAN | Test now: `Start-ScheduledTask -TaskName "Disklordz DAW Inbox"` → console shows `watching…` → download a kit → it appears in the inbox. Remove: `Unregister-ScheduledTask -TaskName "Disklordz DAW Inbox" -Confirm:$false`. cmd.exe variant and no-admin Startup-folder option: [daw-inbox README](../disklordz/daw-inbox/README.md#run-at-login-task-scheduler). |

---

## Still blocked after this checklist

| Blocker | Why | Owner |
|---------|-----|-------|
| **Build** / `windows-vst3` red on all PRs | CI cannot check out the pinned `clap_juce_extensions` commit | Agent run issue (code fix) |
| n8n stub workflows | Credentials can only be entered in the n8n UI | HUMAN |
| Bytebot desktop-ops | External local Docker setup | HUMAN |
| Stripe live mode | Deliberately last (B14) | HUMAN |
