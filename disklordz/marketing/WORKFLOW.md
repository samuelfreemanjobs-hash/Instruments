# Marketing Department — workflow & automation (VARA v1)

## Daily (async)

| Time | Actor | Action |
|------|-------|--------|
| AM | VARA | Scan `#disklordz-marketing-dept` + active Airtable WOs |
| AM | Subagent owner | Post WIP or blocker in thread |
| PM | VARA + PM Agent | If ship window <7d: confirm asset checklist |

## Weekly

- **GTM scorecard** (`mkt-perf`) posted Monday
- **Creative batch**: 3 short-form scripts (`mkt-content`) queued
- **Funnel review**: one A/B idea (`mkt-funnel`)

## Automation roadmap

| Phase | Automation | Owner |
|-------|------------|--------|
| **Now** | Opt-in → redirect TY; optional Resend Day 0 | Engineering + `mkt-lifecycle` spec |
| **P1** | Stripe webhook → tag purchaser → email branch | `[Marketing][Lifecycle]` WO |
| **P1** | UTM → Plausible/PostHog events | `[Marketing][Perf]` + eng |
| **P2** | Airtable campaign row → auto Slack thread | Zapier read-only or script |
| **P2** | Scheduled social via Buffer API | `mkt-content` |
| **P3** | RAG-assisted copy drafts from corpus | `disklordz/rag/` + review by VARA |

## RACI (launch SKU)

| Task | VARA | PM | Engineering | Founder |
|------|------|-----|-------------|---------|
| SKU brief | A | R | C | I |
| Deploy | I | A | R | I |
| Email sequence | R | C | A | I |
| Demo beat | A | I | C | C |
| Pricing | C | A | I | R |
| Legal/samples | C | C | I | A |

R=Responsible, A=Accountable, C=Consulted, I=Informed
