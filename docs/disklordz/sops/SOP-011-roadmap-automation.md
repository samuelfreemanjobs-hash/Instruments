# SOP-011 — Roadmap backlog automation

## What can be scripted (no secrets)

| Backlog item | Automation | Script |
|--------------|------------|--------|
| Partial OSS integrations | Verify wiring paths; promote to `integrated` when files exist | `promote-integrations.py --write` |
| Fleet docs / PM ADD | Regenerate | `sync-disklordz-agent-fleet.sh` |
| Stub agent blockers | List missing env vars | `stub-agent-env-check.sh` |
| SaaS 007 variations | Assert 2 studio / 3 creative variations | `saas-007-smoke.sh` |
| pgvector embed | Dry-run chunk count | `embed_and_upsert.py --dry-run` |
| Roadmap metrics | Refresh progress table | `update-roadmap-progress.py` |

**Master command:**

```bash
./scripts/complete-roadmap-automation.sh
```

CI: `.github/workflows/roadmap-automation.yml` (weekly + manual).

## What cannot be fully automated (needs you)

| Item | Why | Action |
|------|-----|--------|
| n8n billing/churn/analytics | OAuth + credentials live in n8n | Import `disklordz/agents/workflows/n8n/*.json`; connect Stripe/Supabase |
| Bytebot desktop-ops | External Docker + desktop | [BYTEBOT_SETUP.md](../../BYTEBOT_SETUP.md) |
| Inngest prod async | `INNGEST_EVENT_KEY` on Vercel | `register-inngest.sh` + deploy |
| Stripe credits prod | Webhook + live keys | Stripe dashboard + `STRIPE_*` on Vercel |
| pgvector prod embed | `OPENAI_API_KEY` + service role | `activate-integrations.sh` |
| GPU engines | Modal/Replicate URLs | `engines/modal/deploy_stub.py` → real deploy |

See generated [STUB_AGENT_BLOCKERS.md](../../../disklordz/agents/workflows/STUB_AGENT_BLOCKERS.md).
