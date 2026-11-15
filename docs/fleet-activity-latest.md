# Fleet activity (latest MVP motion run)

*Generated:* 2026-11-08T17:13:43Z · agents in registry: **32**

Regenerate: `./scripts/run-agent-fleet-mvp.sh` or `python3 disklordz/agents/workflows/fleet_mvp_motion.py`

| Agent | Status | Detail |
|-------|--------|--------|
| airtable-wo-triage | blocked | Airtable + dispatch webhook |
| analytics-interpreter | blocked | Stripe + Supabase reads |
| async-generation | blocked | INNGEST_EVENT_KEY / INNGEST_SIGNING_KEY |
| billing-ops | blocked | STRIPE_* + n8n billing stub |
| churn-winback | blocked | n8n + Supabase service role |
| competitive-intel | blocked | manual monthly doc pass |
| conversion-qa | blocked | needs DISKLORDZ_URL for go-live smoke |
| daw-inbox-copilot | ok | daw-inbox module parse |
| desktop-ops | blocked | Bytebot external |
| engine-swap | ok | integration deploy stub |
| factory-batch-gpu | ok | integration deploy stub |
| fraud-abuse | blocked | live POST /api/generate traffic |
| golden-wav-qa | ok | run_business.py --profile ci-verify |
| hardware-preset-designer | skip | hardware_preset_designer --self-test (1: Traceback (most recent call last):   File "/workspace/disklordz/integrations... |
| integration-health | ok | promote integrations manifest |
| lane-workflow | ok | spark audio contracts (shared) |
| license-compliance | blocked | human sign-off before engine-swap prod |
| marketing-glue | blocked | n8n docker + credentials |
| onboarding-concierge | blocked | Inngest user.signed_up (same keys) |
| ops-schema | blocked | workflow_dispatch activate-integrations + Supabase |
| pm-agent | ok | stub env checklist |
| pricing-experiment | blocked | human /premortem + Stripe price IDs |
| product-factory | ok | openai_factory_agent stub |
| prompt-coach | ok | embed dry-run |
| referral-affiliate | blocked | Stripe checkout + coupons live |
| release-notes | ok | merged PR digest |
| seo-kit-pages | blocked | Vercel build / ISR kit routes |
| ship-velocity | blocked | repository_dispatch / Airtable token |
| social-clip-factory | blocked | ffmpeg + kit WAV input |
| spec-validator | ok | verify-spark-audio-contracts |
| support-macro | blocked | POST /api/rag/suggest (needs running site) |
| workflow-automation | ok | scaffold_workflows.py |

**Summary:** ok=12 skip=1 blocked=19 fail=0

Prod-only agents stay `blocked` until secrets in `docs/NEXT.md` / `STUB_AGENT_BLOCKERS.md`.
