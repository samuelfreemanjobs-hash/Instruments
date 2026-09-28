# Disklordz agent fleet — activation matrix

Repo-wide companion to [DISKLORDZ_AGENTS.md](../DISKLORDZ_AGENTS.md) and [PM ADD](../disklordz/agents/workflows/PM_ADD.md).

| ID | Title | Activation | Primary automation | Skill |
|----|-------|------------|--------------------|-------|
| `workflow-automation` | Workflow Automation | `ci_on_push` | `.github/workflows/scaffold-agent-workflows.yml` | `.github/skills/disklordz-workflow-automation/SKILL.md` |
| `pm-agent` | PM Agent (Fleet ADD) | `ci_scheduled` | `.github/workflows/agent-fleet-governance.yml` | `.github/skills/disklordz-pm-agent/SKILL.md` |
| `conversion-qa` | Conversion QA | `ci_manual` | `.github/workflows/disklordz-go-live.yml` | `.github/skills/disklordz-conversion-qa/SKILL.md` |
| `billing-ops` | Billing Ops | `manual_stub` | `disklordz/agents/workflows/n8n/billing-ops-stub.json` | `.github/skills/disklordz-billing-ops/SKILL.md` |
| `ship-velocity` | Ship Velocity | `ci_event` | `.github/workflows/airtable-antigravity-handoff.yml` | `.github/skills/disklordz-ship-velocity/SKILL.md` |
| `async-generation` | Async Generation | `runtime_inngest` | `disklordz/website/src/inngest/functions.ts` | `.github/skills/disklordz-async-generation/SKILL.md` |
| `prompt-coach` | Prompt Coach | `ci_on_push` | `.github/workflows/rag-reindex.yml` | `.github/skills/disklordz-prompt-coach/SKILL.md` |
| `product-factory` | Product Factory | `runtime_api` | `disklordz/website/src/app/api/factory/batch/route.ts` | `.github/skills/disklordz-product-factory/SKILL.md` |
| `spec-validator` | Spec Validator | `runtime_api` | `disklordz/integrations/agents/pydantic_spec_agent.py` | `.github/skills/disklordz-spec-validator/SKILL.md` |
| `lane-workflow` | Lane Workflow | `cli` | `disklordz/integrations/agents/langgraph_spec_flow.py` | `.github/skills/disklordz-lane-workflow/SKILL.md` |
| `ops-schema` | Ops Schema | `ci_manual` | `.github/workflows/activate-integrations.yml` | `.github/skills/disklordz-ops-schema/SKILL.md` |
| `marketing-glue` | Marketing Glue | `manual_stub` | `disklordz/agents/workflows/n8n/marketing-glue-stub.json` | `.github/skills/disklordz-marketing-glue/SKILL.md` |
| `support-macro` | Support Macro | `runtime_api` | `disklordz/website/src/app/api/rag/suggest/route.ts` | `.github/skills/disklordz-support-macro/SKILL.md` |
| `desktop-ops` | Desktop Ops | `manual_external` | `docs/BYTEBOT_SETUP.md` | `.github/skills/disklordz-desktop-ops/SKILL.md` |
| `factory-batch-gpu` | Factory Batch GPU | `manual_external` | `disklordz/integrations/engines/README.md` | `.github/skills/disklordz-factory-batch-gpu/SKILL.md` |
| `engine-swap` | Engine Swap | `runtime_env` | `disklordz/integrations/docker-compose.optional.yml` | `.github/skills/disklordz-engine-swap/SKILL.md` |
| `integration-health` | Integration Health | `ci_scheduled` | `.github/workflows/agent-fleet-health.yml` | `.github/skills/disklordz-integration-health/SKILL.md` |
| `churn-winback` | Churn Win-back | `manual_stub` | `disklordz/agents/workflows/n8n/churn-winback-stub.json` | `.github/skills/disklordz-churn-winback/SKILL.md` |
| `seo-kit-pages` | SEO Kit Pages | `runtime_build` | `disklordz/website/next build` | `.github/skills/disklordz-seo-kit-pages/SKILL.md` |
| `referral-affiliate` | Referral & Affiliate | `runtime_stripe` | `STRIPE_PRO_PRICE_ID + coupon` | `.github/skills/disklordz-referral-affiliate/SKILL.md` |
| `fraud-abuse` | Fraud & Abuse | `runtime_api` | `disklordz/website/src/lib/rate-limit.ts` | `.github/skills/disklordz-fraud-abuse/SKILL.md` |
| `pricing-experiment` | Pricing Experiment | `manual_skill` | `.claude/skills/premortem/SKILL.md` | `.github/skills/disklordz-pricing-experiment/SKILL.md` |
| `onboarding-concierge` | Onboarding Concierge | `runtime_inngest` | `disklordz/website/src/inngest/functions.ts` | `.github/skills/disklordz-onboarding-concierge/SKILL.md` |
| `daw-inbox-copilot` | DAW Inbox Copilot | `cli_local` | `disklordz/daw-inbox/package.json` | `.github/skills/disklordz-daw-inbox-copilot/SKILL.md` |
| `airtable-wo-triage` | Airtable WO Triage | `ci_event` | `.github/workflows/airtable-antigravity-handoff.yml` | `.github/skills/disklordz-airtable-wo-triage/SKILL.md` |
| `golden-wav-qa` | Golden WAV QA | `ci_on_pr` | `.github/workflows/build.yml` | `.github/skills/disklordz-golden-wav-qa/SKILL.md` |
| `competitive-intel` | Competitive Intel | `manual_doc` | `docs/DISKLORDZ_ILLUGEN_RESEARCH.md` | `.github/skills/disklordz-competitive-intel/SKILL.md` |
| `license-compliance` | License & Compliance | `manual_gate` | `disklordz/agents/profit/license-compliance/agent.md` | `.github/skills/disklordz-license-compliance/SKILL.md` |
| `social-clip-factory` | Social Clip Factory | `cli_script` | `disklordz/agents/workflows/scripts/social-clip-stub.sh` | `.github/skills/disklordz-social-clip-factory/SKILL.md` |
| `analytics-interpreter` | Analytics Interpreter | `manual_stub` | `disklordz/agents/workflows/n8n/analytics-weekly-stub.json` | `.github/skills/disklordz-analytics-interpreter/SKILL.md` |
| `release-notes` | Release Notes | `ci_script` | `disklordz/agents/workflows/scripts/release-notes-from-prs.sh` | `.github/skills/disklordz-release-notes/SKILL.md` |
| `hardware-preset-designer` | Hardware Preset Designer | `ci_on_pr` | `.github/workflows/synth-forge.yml` | `.github/skills/disklordz-hardware-preset-designer/SKILL.md` |
| `audio-plugin-coder` | Audio Plugin Coder (APC) | `manual_doc` | `TBD` | `.github/skills/disklordz-audio-plugin-coder/SKILL.md` |
| `code-project-planner` | Code Project Planner (PRD) | `manual_doc` | `TBD` | `.github/skills/disklordz-code-project-planner/SKILL.md` |
| `ddsp-ml-engineer` | DDSP / ML Engineer (PyTorch) | `ci_on_pr` | `TBD` | `.github/skills/disklordz-ddsp-ml-engineer/SKILL.md` |

### Activation legend

| Code | Meaning |
|------|---------|
| `ci_scheduled` | GitHub Actions cron runs fleet health / governance |
| `ci_on_push` | Workflow runs when mapped paths change |
| `ci_on_pr` | Plugin / website CI on pull request |
| `runtime_api` | Live on Vercel API routes |
| `runtime_inngest` | Inngest functions in production |
| `manual_stub` | n8n JSON stub — import + secrets required |

Regenerate: `./scripts/sync-disklordz-agent-fleet.sh`
