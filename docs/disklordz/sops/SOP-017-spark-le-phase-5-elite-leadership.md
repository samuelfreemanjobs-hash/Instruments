# SOP-017 — SparkLE Phase 5: Elite leadership (proactive, sustainable)

**Phase:** 5 · **Charter:** [SOP-012](SOP-012-spark-le-elite-system-charter.md) · **Version:** 1.0

## Purpose

The business **leads ahead of demand** — predictable elite drops, clear roadmap, safe automation — so growth does not kill quality, people, or trust.

“No one dies” at Phase 5 means: **sustainable cadence**, **gates honored**, **metrics talked about honestly**.

## Prerequisites

- Phases 1–4 operating (even if some tooling still manual).
- Keeper defaults and contracts stable for ≥ 4 weeks.

## Cadence (follow every period)

### Weekly (30–45 min producer + 15 min builder)

| Step | Action |
|------|--------|
| 1 | Run one lane session ([SOP-013](SOP-013-spark-le-phase-1-feed-spark.md) + review template) |
| 2 | Read KPIs: keeper rate, time-to-first-beat, contract failures |
| 3 | Pick **one** fix tagged last week |
| 4 | Log decision: ship / hold / revert |

### Monthly (60 min)

| Step | Action |
|------|--------|
| 1 | **Family audit** per [ELITE-PERFORMANCE.md](../spark-le/ELITE-PERFORMANCE.md) — one family/week; 2 lanes @ same BPM |
| 2 | Rotate keepers (demote bottom 2 if corpus full) |
| 3 | Update [ROADMAP.md](../../ROADMAP.md) spark-le bullet ([SOP-008](SOP-008-roadmap-and-progress.md)) |
| 4 | Retrospective: revise one phase SOP if repeated friction |

### Quarterly (half day)

| Step | Action |
|------|--------|
| 1 | Blind A/B defaults vs challenger lineage |
| 2 | RAG corpus refresh for lane language ([SOP-007](SOP-007-rag-corpus.md)) |
| 3 | Security pass: no secrets, no uncleared samples in keepers |
| 4 | Optional: agent fleet smoke if spark-le wired to automation ([SOP-003](SOP-003-run-agent-fleet.md)) |

## Proactive leadership rules

1. **Default forward** — Next pack spec written **before** ears are tired (BPM, lane, mutation parent).
2. **Hold line on red** — CI, contracts, or safety table → stop downstream marketing/generate.
3. **Name the lane** — External copy says **hard gangsta trap**, phonk, vintage south; avoid mislabels.
4. **Mission alignment** — Music “for the good”: document intent in keeper notes (who it serves, what it avoids glorifying); no automated moralizing in product — **human statement** in release notes when public.
5. **Delegate safely** — Agents implement; humans confirm prod, payments, schema, external posts.

## KPI dashboard (minimum)

Track in spreadsheet or ROADMAP metrics:

- Time to first beat (median)
- Keeper rate by lane
- Contract failure count / month
- Review sessions completed / month
- Customer incidents (generate/download) → [TEMPLATE-incident-debug.md](../templates/TEMPLATE-incident-debug.md)

## Success criteria (Phase 5 “operating elite”)

- [ ] 8+ weeks without silent quality regression (contracts + keeper spot check)
- [ ] Weekly cadence met ≥ 3 of last 4 weeks
- [ ] One proactive drop planned 2 weeks ahead
- [ ] Zero prod deploys outside [SOP-005](SOP-005-go-live-verify.md) + human OK

## Improvement loop

- This SOP is the **capstone** — edit cadence only if burnout signals (missed reviews, rising clips, tag `safety`).
- Adding SaaS surface for spark-le packs → new product ARCHITECTURE + SOP-005 path before launch.

## Escalation

- Revenue pressure to skip keeper bar → charter safety table overrides; document dissent in PR if overridden by owner.

**SOP-017 v1.0**
