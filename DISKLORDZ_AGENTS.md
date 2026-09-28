# Disklordz agent fleet (repo-wide)

Company-wide index for **Instruments / Disklordz**. Every Cursor Cloud Agent, Claude Code session, and engineer should discover agents here — not only under `disklordz/agents/`.

| Resource | Purpose |
|----------|---------|
| [PM ADD roster](disklordz/agents/workflows/PM_ADD.md) | Who we are + automation pointers |
| [Agent fleet table](docs/DISKLORDZ_AGENT_FLEET.md) | Activation status + paths |
| [`.github/agents/fleet.json`](.github/agents/fleet.json) | Machine index for CI and tooling |
| [Profit trees](disklordz/agents/profit/) | `agent.md`, `skill.md`, … per agent |
| [GitHub Copilot skills](.github/skills/) | `disklordz-<id>/SKILL.md` |

## Orchestration (read first)

| ID | Role |
|----|------|
| **workflow-automation** | Scaffolds PM ADD + per-agent `automation.yaml` |
| **pm-agent** | Repo-wide fleet governance + scheduled execution checks |

**Fleet size:** 31 agents (includes orchestration).

## Active CI (executing now)

| Workflow | Serves |
|----------|--------|
| `.github/workflows/agent-fleet-governance.yml` | pm-agent, workflow-automation |
| `.github/workflows/agent-fleet-health.yml` | integration-health, pm-agent |
| `.github/workflows/agent-fleet-execute.yml` | pm-agent + executable fleet roles |
| `.github/workflows/scaffold-agent-workflows.yml` | workflow-automation |
| `.github/workflows/disklordz-go-live.yml` | conversion-qa, ops-schema |
| `.github/workflows/rag-reindex.yml` | prompt-coach |
| `.github/workflows/activate-integrations.yml` | ops-schema, integration-health |
| `.github/workflows/airtable-antigravity-handoff.yml` | ship-velocity, airtable-wo-triage |
| `.github/workflows/build.yml` | golden-wav-qa |

## Regenerate (Workflow Automation + PM Agent)

```bash
./scripts/sync-disklordz-agent-fleet.sh
```

Maintained by `scaffold_workflows.py` — do not hand-edit sections below the marker.

<!-- FLEET_ROSTER_BEGIN -->

- `workflow-automation` — **Workflow Automation** (`ci_on_push`)
- `pm-agent` — **PM Agent (Fleet ADD)** (`ci_scheduled`)
- `conversion-qa` — **Conversion QA** (`ci_manual`)
- `billing-ops` — **Billing Ops** (`manual_stub`)
- `ship-velocity` — **Ship Velocity** (`ci_event`)
- `async-generation` — **Async Generation** (`runtime_inngest`)
- `prompt-coach` — **Prompt Coach** (`ci_on_push`)
- `product-factory` — **Product Factory** (`runtime_api`)
- `spec-validator` — **Spec Validator** (`runtime_api`)
- `lane-workflow` — **Lane Workflow** (`cli`)
- `ops-schema` — **Ops Schema** (`ci_manual`)
- `marketing-glue` — **Marketing Glue** (`manual_stub`)
- `support-macro` — **Support Macro** (`runtime_api`)
- `desktop-ops` — **Desktop Ops** (`manual_external`)
- `factory-batch-gpu` — **Factory Batch GPU** (`manual_external`)
- `engine-swap` — **Engine Swap** (`runtime_env`)
- `integration-health` — **Integration Health** (`ci_scheduled`)
- `churn-winback` — **Churn Win-back** (`manual_stub`)
- `seo-kit-pages` — **SEO Kit Pages** (`runtime_build`)
- `referral-affiliate` — **Referral & Affiliate** (`runtime_stripe`)
- `fraud-abuse` — **Fraud & Abuse** (`runtime_api`)
- `pricing-experiment` — **Pricing Experiment** (`manual_skill`)
- `onboarding-concierge` — **Onboarding Concierge** (`runtime_inngest`)
- `daw-inbox-copilot` — **DAW Inbox Copilot** (`cli_local`)
- `airtable-wo-triage` — **Airtable WO Triage** (`ci_event`)
- `golden-wav-qa` — **Golden WAV QA** (`ci_on_pr`)
- `competitive-intel` — **Competitive Intel** (`manual_doc`)
- `license-compliance` — **License & Compliance** (`manual_gate`)
- `social-clip-factory` — **Social Clip Factory** (`cli_script`)
- `analytics-interpreter` — **Analytics Interpreter** (`manual_stub`)
- `release-notes` — **Release Notes** (`ci_script`)

<!-- FLEET_ROSTER_END -->
