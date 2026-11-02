# Profit agents (31 implemented)

**Repo-wide index:** [DISKLORDZ_AGENTS.md](../DISKLORDZ_AGENTS.md) · **PM ADD:** [disklordz/agents/workflows/PM_ADD.md](../disklordz/agents/workflows/PM_ADD.md)

Elite agent file trees: **`disklordz/agents/profit/<id>/`** (8 files each). Bound by **[META v3.1 charter](../CLAUDE.md)** — sync via `./scripts/sync-meta-llm-charter.sh`.

| File | Role |
|------|------|
| `agent.md` | Frontmatter + system prompt |
| `skill.md` | Two-phase skill |
| `subagents.md` | Coordinator / explore / verify / implement |
| `soul.md` | Principles + charter reference |
| `hooks.md` | Lifecycle hooks |
| `memory.md` | File-tier MEMORY |
| `tools.md` | Allowlist + MCP |
| `loop.md` | AsyncGenerator SOP |

Registry: [`disklordz/agents/profit/registry.json`](../disklordz/agents/profit/registry.json)  
Copilot skills: `.github/skills/disklordz-<id>/SKILL.md`

## Regenerate

```bash
./scripts/sync-disklordz-agent-fleet.sh   # fleet entrypoints + PM ADD + skills
./scripts/sync-meta-llm-charter.sh        # optional: refresh META core + /weave skills
```

## Orchestration

| ID | Title |
|----|-------|
| workflow-automation | Workflow Automation |
| pm-agent | PM Agent (Fleet ADD) |

## Fleet (1–15)

| # | ID | Title |
|---|-----|-------|
| 1 | conversion-qa | Conversion QA |
| 2 | billing-ops | Billing Ops |
| 3 | ship-velocity | Ship Velocity |
| 4 | async-generation | Async Generation |
| 5 | prompt-coach | Prompt Coach |
| 6 | product-factory | Product Factory |
| 7 | spec-validator | Spec Validator |
| 8 | lane-workflow | Lane Workflow |
| 9 | ops-schema | Ops Schema |
| 10 | marketing-glue | Marketing Glue |
| 11 | support-macro | Support Macro |
| 12 | desktop-ops | Desktop Ops |
| 13 | factory-batch-gpu | Factory Batch GPU |
| 14 | engine-swap | Engine Swap |
| 15 | integration-health | Integration Health |

## Fleet (16–29)

| # | ID | Title | Profit focus |
|---|-----|-------|----------------|
| 16 | churn-winback | Churn Win-back | Reactivate lapsed Pro |
| 17 | seo-kit-pages | SEO Kit Pages | Organic → signups |
| 18 | referral-affiliate | Referral & Affiliate | Lower CAC |
| 19 | fraud-abuse | Fraud & Abuse | Free-tier margin |
| 20 | pricing-experiment | Pricing Experiment | ARPU / Stripe A/B |
| 21 | onboarding-concierge | Onboarding Concierge | Time-to-first-kit |
| 22 | daw-inbox-copilot | DAW Inbox Copilot | Producer stickiness |
| 23 | airtable-wo-triage | Airtable WO Triage | PM throughput |
| 24 | golden-wav-qa | Golden WAV QA | Plugin cross-sell quality |
| 25 | competitive-intel | Competitive Intel | vs ILLUGEN |
| 26 | license-compliance | License & Compliance | Engine licensing |
| 27 | social-clip-factory | Social Clip Factory | Top-of-funnel clips |
| 28 | analytics-interpreter | Analytics Interpreter | MRR / gen metrics |
| 29 | release-notes | Release Notes | Trust + upgrades |

Phase 3 ideas: [DISKLORDZ_PROFIT_AGENTS_EXTENDED.md](DISKLORDZ_PROFIT_AGENTS_EXTENDED.md)
