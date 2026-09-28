# Next — after “run the business” is green

Use this when local **`run_business.py --profile full`** passes and you want the **production** tranche.

## One command (local)

```bash
./scripts/run-next.sh
```

Runs: roadmap automation → Inngest sync URL probe → go-live smoke → `run_business.py --profile ci-verify`.

## Production checklist (human / Vercel)

| Priority | Secret / action | Unblocks |
|----------|-----------------|----------|
| 1 | `DISKLORDZ_VERIFY_BASE_URL` in GitHub | Fleet CI prod smoke |
| 2 | `INNGEST_EVENT_KEY`, `INNGEST_SIGNING_KEY` on Vercel | Async generation agent |
| 3 | `STRIPE_*` + webhook | Billing ops, credits ledger |
| 4 | `OPENAI_API_KEY` + Supabase service role | pgvector embed (`embed_and_upsert.py`) |
| 5 | n8n: `docker compose -f disklordz/integrations/docker-compose.optional.yml --profile n8n up -d` + import JSON stubs | Marketing, churn, analytics agents |

Details: [disklordz/agents/workflows/STUB_AGENT_BLOCKERS.md](../disklordz/agents/workflows/STUB_AGENT_BLOCKERS.md)

## SaaS code status (in git)

WO **007–016** features are implemented (spec UI, variations, factory batch, daw-inbox, etc.). See [DISKLORDZ_ILLUGEN_RESEARCH.md](DISKLORDZ_ILLUGEN_RESEARCH.md) phased table.

**Next product WOs (ideas):** phase-3 profit agents in [DISKLORDZ_PROFIT_AGENTS_EXTENDED.md](DISKLORDZ_PROFIT_AGENTS_EXTENDED.md) · go-live [DISKLORDZ_GO_LIVE.md](DISKLORDZ_GO_LIVE.md).

## Plugin lane

- JD Upgraded: [PHASE5.md](PHASE5.md)
- WAVE-9090: ship build from `run_business` pluginval stage
