# Disklordz — complete setup (agents + SaaS)

One checklist to run the **business** after merging PRs with WO-SAAS-018–028.

## 1. GitHub Actions secrets

| Secret | Required for |
|--------|----------------|
| `CURSOR_API_KEY` | All Cloud Agent daily/weekly PRs |
| `DISKLORDZ_URL` | Live go-live smoke (WO-019) |
| `SLACK_WEBHOOK_URL` | Optional CI/daily summaries |
| `SUPABASE_*` + Stripe | Production SaaS (see [DISKLORDZ_GO_LIVE_SECRETS.md](DISKLORDZ_GO_LIVE_SECRETS.md)) |

## 2. Vercel env (website)

Copy from `disklordz/website/.env.example`, including:

- `SUPABASE_SERVICE_ROLE_KEY` — kits, analytics events, async jobs
- `OPS_API_KEY` — `/api/ops/analytics-summary` (WO-025)

## 3. Supabase migrations

```bash
cd disklordz/website
npx supabase db push   # or CI migrate workflow
```

Includes `generation_events`, `generation_jobs` (WO-025/026).

## 4. Verify locally

```bash
./scripts/disklordz-business-agents.sh check
./scripts/disklordz-factory-daily.sh --skip-agent
cd disklordz/website && npm run saas:ops:check
```

## 5. Enable schedules

Workflows on `main` (UTC):

| Time | Workflow |
|------|----------|
| 12:30 | Factory DSP daily |
| 13:00 | SaaS ops daily |
| Mon 14:00 | Billing weekly |
| Wed 14:00 | Growth weekly |
| Fri 14:00 | Preset curator weekly |
| 1st Mon 15:00 | Competitive intel monthly |

## 6. PM router (optional)

```bash
gh api repos/samuelfreemanjobs-hash/Instruments/dispatches \
  -f event_type=disklordz-pm-router \
  -f 'client_payload[intake]=Stripe checkout ok but Pro not active'
```

**Index:** [DISKLORDZ_BUSINESS_AGENTS.md](DISKLORDZ_BUSINESS_AGENTS.md)
