# PM ADD — Profit agent roster

Product management **ADD** board: who we are, how we help Disklordz, and where the automation lives.

Maintained by **Workflow Automation** — regenerate:

```bash
python3 disklordz/agents/workflows/scaffold_workflows.py
```

---

## workflow-automation

I'm **Workflow Automation** — I wire GitHub Actions, Inngest, and n8n stubs so every profit agent runs on a schedule or event without you babysitting scripts.

- **Automation:** `.github/workflows/scaffold-agent-workflows.yml` (github, workflow_dispatch)
- **Agent docs:** [`profit/workflow-automation/agent.md`](../profit/workflow-automation/agent.md)
- **Workflow file:** [`agents/workflow-automation/automation.yaml`](agents/workflow-automation/automation.yaml)

## conversion-qa

I'm **Conversion QA** — I run go-live smoke and Playwright checks so generate → preview → checkout never silently breaks after a deploy.

- **Automation:** `.github/workflows/disklordz-go-live.yml` (github, schedule + post-deploy)
- **Agent docs:** [`profit/conversion-qa/agent.md`](../profit/conversion-qa/agent.md)
- **Workflow file:** [`agents/conversion-qa/automation.yaml`](agents/conversion-qa/automation.yaml)

## billing-ops

I'm **Billing Ops** — I watch Stripe credits, failed payments, and Pro status so revenue leaks get flagged before users churn.

- **Automation:** `disklordz/automation/n8n/billing-ops-stub.json` (manual, weekly + stripe webhook)
- **Agent docs:** [`profit/billing-ops/agent.md`](../profit/billing-ops/agent.md)
- **Workflow file:** [`agents/billing-ops/automation.yaml`](agents/billing-ops/automation.yaml)

## ship-velocity

I'm **Ship Velocity** — I turn Airtable WOs into issues and PRs fast so SaaS features reach production while the idea is still hot.

- **Automation:** `.github/workflows/airtable-antigravity-handoff.yml` (github, repository_dispatch)
- **Agent docs:** [`profit/ship-velocity/agent.md`](../profit/ship-velocity/agent.md)
- **Workflow file:** [`agents/ship-velocity/automation.yaml`](agents/ship-velocity/automation.yaml)

## async-generation

I'm **Async Generation** — I queue long kit jobs through Inngest so paid generations finish even when Vercel would time out.

- **Automation:** `disklordz/website/src/inngest/functions.ts` (inngest, disklordz/generate.requested)
- **Agent docs:** [`profit/async-generation/agent.md`](../profit/async-generation/agent.md)
- **Workflow file:** [`agents/async-generation/automation.yaml`](agents/async-generation/automation.yaml)

## prompt-coach

I'm **Prompt Coach** — I use RAG and lane docs to sharpen prompts and specs so free users hear their vibe in the WAVs and upgrade.

- **Automation:** `.github/workflows/rag-reindex.yml` (github, push docs/**)
- **Agent docs:** [`profit/prompt-coach/agent.md`](../profit/prompt-coach/agent.md)
- **Workflow file:** [`agents/prompt-coach/automation.yaml`](agents/prompt-coach/automation.yaml)

## product-factory

I'm **Product Factory** — I batch storefront packs (KICKS/PERC folders) so you ship SKUs with metadata, not one-off ZIPs.

- **Automation:** `disklordz/website/src/app/api/factory/batch/route.ts` (api, POST /api/factory/batch)
- **Agent docs:** [`profit/product-factory/agent.md`](../profit/product-factory/agent.md)
- **Workflow file:** [`agents/product-factory/automation.yaml`](agents/product-factory/automation.yaml)

## spec-validator

I'm **Spec Validator** — I reject bad GenerationSpec JSON before generate runs so you don't burn credits on nonsense BPM/key combos.

- **Automation:** `disklordz/integrations/agents/pydantic_spec_agent.py` (api, pre-generate)
- **Agent docs:** [`profit/spec-validator/agent.md`](../profit/spec-validator/agent.md)
- **Workflow file:** [`agents/spec-validator/automation.yaml`](agents/spec-validator/automation.yaml)

## lane-workflow

I'm **Lane Workflow** — I drive prompt → spec → variations so phonk/drift/cyber lanes stay consistent across candidates.

- **Automation:** `disklordz/integrations/agents/langgraph_spec_flow.py` (script, cli)
- **Agent docs:** [`profit/lane-workflow/agent.md`](../profit/lane-workflow/agent.md)
- **Workflow file:** [`agents/lane-workflow/automation.yaml`](agents/lane-workflow/automation.yaml)

## ops-schema

I'm **Ops Schema** — I apply and verify Supabase migrations safely so auth, kits, and pgvector stay up while you ship schema.

- **Automation:** `.github/workflows/activate-integrations.yml` (github, workflow_dispatch)
- **Agent docs:** [`profit/ops-schema/agent.md`](../profit/ops-schema/agent.md)
- **Workflow file:** [`agents/ops-schema/automation.yaml`](agents/ops-schema/automation.yaml)

## marketing-glue

I'm **Marketing Glue** — I connect n8n flows for leads and email so traffic comes back without manual copy-paste.

- **Automation:** `disklordz/agents/workflows/n8n/marketing-glue-stub.json` (n8n, cron)
- **Agent docs:** [`profit/marketing-glue/agent.md`](../profit/marketing-glue/agent.md)
- **Workflow file:** [`agents/marketing-glue/automation.yaml`](agents/marketing-glue/automation.yaml)

## support-macro

I'm **Support Macro** — I draft answers from your lane corpus so support is fast, accurate, and cheap.

- **Automation:** `disklordz/website/src/app/api/rag/suggest/route.ts` (api, POST /api/rag/suggest)
- **Agent docs:** [`profit/support-macro/agent.md`](../profit/support-macro/agent.md)
- **Workflow file:** [`agents/support-macro/automation.yaml`](agents/support-macro/automation.yaml)

## desktop-ops

I'm **Desktop Ops** — I handle browser/vendor chores via Bytebot while you keep code changes in git where they belong.

- **Automation:** `docs/BYTEBOT_SETUP.md` (external, manual)
- **Agent docs:** [`profit/desktop-ops/agent.md`](../profit/desktop-ops/agent.md)
- **Workflow file:** [`agents/desktop-ops/automation.yaml`](agents/desktop-ops/automation.yaml)

## factory-batch-gpu

I'm **Factory Batch GPU** — I orchestrate heavy catalog batches off the serverless path so big drops don't choke the site.

- **Automation:** `disklordz/integrations/engines/README.md` (trigger.dev, long batch)
- **Agent docs:** [`profit/factory-batch-gpu/agent.md`](../profit/factory-batch-gpu/agent.md)
- **Workflow file:** [`agents/factory-batch-gpu/automation.yaml`](agents/factory-batch-gpu/automation.yaml)

## engine-swap

I'm **Engine Swap** — I manage remote AudioCraft/Stable workers with parametric fallback so Pro sound improves without bricking generate.

- **Automation:** `disklordz/integrations/docker-compose.optional.yml` (docker, DISKLORDZ_ENGINE=remote)
- **Agent docs:** [`profit/engine-swap/agent.md`](../profit/engine-swap/agent.md)
- **Workflow file:** [`agents/engine-swap/automation.yaml`](agents/engine-swap/automation.yaml)

## integration-health

I'm **Integration Health** — I hit `/api/integrations/status` and verify scripts so misconfigured env vars don't eat revenue overnight.

- **Automation:** `.github/workflows/agent-fleet-health.yml` (github, daily)
- **Agent docs:** [`profit/integration-health/agent.md`](../profit/integration-health/agent.md)
- **Workflow file:** [`agents/integration-health/automation.yaml`](agents/integration-health/automation.yaml)

## churn-winback

I'm **Churn Win-back** — I find lapsed Pro users and trigger opt-in win-back with credits so MRR recovers.

- **Automation:** `disklordz/agents/workflows/n8n/churn-winback-stub.json` (n8n, weekly)
- **Agent docs:** [`profit/churn-winback/agent.md`](../profit/churn-winback/agent.md)
- **Workflow file:** [`agents/churn-winback/automation.yaml`](agents/churn-winback/automation.yaml)

## seo-kit-pages

I'm **SEO Kit Pages** — I turn factory metadata into indexable pages so organic search feeds signups.

- **Automation:** `disklordz/website/next build` (vercel, build)
- **Agent docs:** [`profit/seo-kit-pages/agent.md`](../profit/seo-kit-pages/agent.md)
- **Workflow file:** [`agents/seo-kit-pages/automation.yaml`](agents/seo-kit-pages/automation.yaml)

## referral-affiliate

I'm **Referral & Affiliate** — I design referral hooks in manifests and Stripe coupons so customers bring customers.

- **Automation:** `STRIPE_PRO_PRICE_ID + coupon` (stripe, checkout)
- **Agent docs:** [`profit/referral-affiliate/agent.md`](../profit/referral-affiliate/agent.md)
- **Workflow file:** [`agents/referral-affiliate/automation.yaml`](agents/referral-affiliate/automation.yaml)

## fraud-abuse

I'm **Fraud & Abuse** — I tighten rate limits and guest abuse heuristics so free tier doesn't become a free CDN for bots.

- **Automation:** `disklordz/website/src/lib/rate-limit.ts` (api, every POST /api/generate)
- **Agent docs:** [`profit/fraud-abuse/agent.md`](../profit/fraud-abuse/agent.md)
- **Workflow file:** [`agents/fraud-abuse/automation.yaml`](agents/fraud-abuse/automation.yaml)

## pricing-experiment

I'm **Pricing Experiment** — I structure Stripe price tests with /premortem discipline so ARPU moves with evidence.

- **Automation:** `.claude/skills/premortem/SKILL.md` (manual, /premortem)
- **Agent docs:** [`profit/pricing-experiment/agent.md`](../profit/pricing-experiment/agent.md)
- **Workflow file:** [`agents/pricing-experiment/automation.yaml`](agents/pricing-experiment/automation.yaml)

## onboarding-concierge

I'm **Onboarding Concierge** — I guide magic-link → first kit so new users hear value in minutes, not days.

- **Automation:** `disklordz/website/src/inngest/functions.ts` (inngest, auth signup)
- **Agent docs:** [`profit/onboarding-concierge/agent.md`](../profit/onboarding-concierge/agent.md)
- **Workflow file:** [`agents/onboarding-concierge/automation.yaml`](agents/onboarding-concierge/automation.yaml)

## daw-inbox-copilot

I'm **DAW Inbox Copilot** — I keep Downloads → inbox → DAW smooth so kits land where producers actually work.

- **Automation:** `disklordz/daw-inbox/package.json` (cli, local watch)
- **Agent docs:** [`profit/daw-inbox-copilot/agent.md`](../profit/daw-inbox-copilot/agent.md)
- **Workflow file:** [`agents/daw-inbox-copilot/automation.yaml`](agents/daw-inbox-copilot/automation.yaml)

## airtable-wo-triage

I'm **Airtable WO Triage** — I route work orders to GitHub and Antigravity inbox JSON so PM and eng stay in sync.

- **Automation:** `.github/workflows/airtable-antigravity-handoff.yml` (github, repository_dispatch)
- **Agent docs:** [`profit/airtable-wo-triage/agent.md`](../profit/airtable-wo-triage/agent.md)
- **Workflow file:** [`agents/airtable-wo-triage/automation.yaml`](agents/airtable-wo-triage/automation.yaml)

## golden-wav-qa

I'm **Golden WAV QA** — I run DSP golden regression so plugin quality backs your brand when you cross-sell Instruments.

- **Automation:** `.github/workflows/build.yml` (github, pull_request paths Source/**)
- **Agent docs:** [`profit/golden-wav-qa/agent.md`](../profit/golden-wav-qa/agent.md)
- **Workflow file:** [`agents/golden-wav-qa/automation.yaml`](agents/golden-wav-qa/automation.yaml)

## competitive-intel

I'm **Competitive Intel** — I map ILLUGEN-shaped gaps to your WO backlog with tagged evidence, not hype.

- **Automation:** `docs/DISKLORDZ_ILLUGEN_RESEARCH.md` (manual, monthly)
- **Agent docs:** [`profit/competitive-intel/agent.md`](../profit/competitive-intel/agent.md)
- **Workflow file:** [`agents/competitive-intel/automation.yaml`](agents/competitive-intel/automation.yaml)

## license-compliance

I'm **License & Compliance** — I gate engine swaps on model licenses so commercial launch doesn't inherit NC surprises.

- **Automation:** `disklordz/agents/profit/license-compliance/agent.md` (gate, pre-engine-swap)
- **Agent docs:** [`profit/license-compliance/agent.md`](../profit/license-compliance/agent.md)
- **Workflow file:** [`agents/license-compliance/automation.yaml`](agents/license-compliance/automation.yaml)

## social-clip-factory

I'm **Social Clip Factory** — I cut short previews from kits for social so top-of-funnel content scales.

- **Automation:** `disklordz/agents/workflows/scripts/social-clip-stub.sh` (script, manual)
- **Agent docs:** [`profit/social-clip-factory/agent.md`](../profit/social-clip-factory/agent.md)
- **Workflow file:** [`agents/social-clip-factory/automation.yaml`](agents/social-clip-factory/automation.yaml)

## analytics-interpreter

I'm **Analytics Interpreter** — I summarize weekly MRR, gen volume, and credit burn from Stripe + Supabase with [executed] numbers.

- **Automation:** `disklordz/agents/workflows/n8n/analytics-weekly-stub.json` (n8n, weekly Monday)
- **Agent docs:** [`profit/analytics-interpreter/agent.md`](../profit/analytics-interpreter/agent.md)
- **Workflow file:** [`agents/analytics-interpreter/automation.yaml`](agents/analytics-interpreter/automation.yaml)

## release-notes

I'm **Release Notes** — I turn merged PRs into customer-facing changelog lines so upgrades feel trustworthy.

- **Automation:** `disklordz/agents/workflows/scripts/release-notes-from-prs.sh` (github, release published)
- **Agent docs:** [`profit/release-notes/agent.md`](../profit/release-notes/agent.md)
- **Workflow file:** [`agents/release-notes/automation.yaml`](agents/release-notes/automation.yaml)
