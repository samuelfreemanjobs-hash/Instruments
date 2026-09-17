# PM Agent — scheduling & automation ownership

**PM Agent** = Business Planner + Airtable Disklordz OS.

Marketing makes noise; **PM makes time and automation real.**

## PM must schedule / configure (one-time per campaign)

| Variable / action | Purpose |
|-------------------|---------|
| `LAUNCH_CAMPAIGN_START` | ISO timestamp — Day 1; tripwire copy auto-enables Day ≥2 |
| `LAUNCH_PUBLIC_BASE_URL` | Production origin for email links |
| `LAUNCH_VOCAL_STEMS_URL` | Real download ZIP |
| `LAUNCH_TRIPWIRE_SAMPLE_URL` | Stripe Payment Link or product URL (Day 2+) |
| `RESEND_API_KEY` + `RESEND_FROM_EMAIL` | Email delivery |
| `SUPABASE_SERVICE_ROLE_KEY` + run migration `20260918000000_launch_automation.sql` | Queue persistence |
| `CRON_SECRET` | Protect `/api/cron/launch-queue` |
| `AUTOMATION_SECRET` | Protect `/api/automation/register-agent` |
| Vercel Cron | Enabled (hourly in `vercel.json`) |

## PM verifies (checklist)

- [ ] Test opt-in → row in `launch_leads` + 4 rows in `launch_scheduled_emails`
- [ ] Cron returns `{ ok: true, sent: N }` in logs
- [ ] New subagent push triggers `forge-agent-registry` workflow

## When FORGE posts a gap

Create Airtable WO `[Eng][Automation]` with acceptance criteria from FORGE RFC.

## Escalation

If Resend or Cron blocked on plan limits, PM chooses: upgrade Vercel, external cron (GitHub Actions ping), or Loops.so migration WO.
