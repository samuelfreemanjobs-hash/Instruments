# Stub agent blockers (auto-generated)

Regenerate: `bash disklordz/agents/workflows/scripts/stub-agent-env-check.sh`

| Agent area | Requirement | This shell |
|------------|-------------|------------|
| billing-ops | `STRIPE_SECRET_KEY` + n8n import | Stripe: missing |
| churn-winback | Supabase + email provider in n8n | SUPABASE_SERVICE: missing |
| analytics-interpreter | Stripe + Supabase read | see above |
| marketing-glue | n8n docker + webhook URL | run: `docker compose -f disklordz/integrations/docker-compose.optional.yml --profile n8n up -d` |
| desktop-ops | Bytebot local | docs/BYTEBOT_SETUP.md (external) |
| factory-batch-gpu | Modal/Trigger/GPU URL | MODAL: missing |
| async-generation / onboarding | Inngest prod keys | INNGEST_EVENT_KEY: missing |
| prompt-coach pgvector | OpenAI + Supabase | OPENAI: missing |

## n8n stub imports

```bash
ls disklordz/agents/workflows/n8n/*.json
# Import in n8n UI; set credentials; enable workflows
```

Cannot automate credential entry — use host secrets / GitHub Actions secrets for CI smoke only.
