#!/usr/bin/env bash
# Report env/secrets blocking unattended stub agents (n8n, Stripe, Bytebot, etc.)
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../../../.." && pwd)"
OUT="${ROOT}/disklordz/agents/workflows/STUB_AGENT_BLOCKERS.md"

check() { if [[ -n "${!1:-}" ]]; then echo "set"; else echo "missing"; fi; }

cat > "$OUT" << EOF
# Stub agent blockers (auto-generated)

Regenerate: \`bash disklordz/agents/workflows/scripts/stub-agent-env-check.sh\`

| Agent area | Requirement | This shell |
|------------|-------------|------------|
| billing-ops | \`STRIPE_SECRET_KEY\` + n8n import | Stripe: $(check STRIPE_SECRET_KEY) |
| churn-winback | Supabase + email provider in n8n | SUPABASE_SERVICE: $(check SUPABASE_SERVICE_ROLE_KEY) |
| analytics-interpreter | Stripe + Supabase read | see above |
| marketing-glue | n8n docker + webhook URL | run: \`docker compose -f disklordz/integrations/docker-compose.optional.yml --profile n8n up -d\` |
| desktop-ops | Bytebot local | docs/BYTEBOT_SETUP.md (external) |
| factory-batch-gpu | Modal/Trigger/GPU URL | MODAL: $(check MODAL_TOKEN_ID) |
| async-generation / onboarding | Inngest prod keys | INNGEST_EVENT_KEY: $(check INNGEST_EVENT_KEY) |
| prompt-coach pgvector | OpenAI + Supabase | OPENAI: $(check OPENAI_API_KEY) |

## n8n stub imports

\`\`\`bash
ls disklordz/agents/workflows/n8n/*.json
# Import in n8n UI; set credentials; enable workflows
\`\`\`

Cannot automate credential entry — use host secrets / GitHub Actions secrets for CI smoke only.
EOF

echo "Wrote $OUT"
