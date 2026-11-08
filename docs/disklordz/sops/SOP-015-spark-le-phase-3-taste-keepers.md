# SOP-015 — SparkLE Phase 3: Taste (keepers & scores)

**Phase:** 3 · **Charter:** [SOP-012](SOP-012-spark-le-elite-system-charter.md) · **Version:** 1.0

## Purpose

Convert **your ear** into **durable business intelligence** — what is elite hard gangsta trap vs noise — without guessing in the next session.

Automation cannot fully score menace and pocket. This phase makes human judgment **structured and cumulative**.

## Prerequisites

- Phase 1 sessions running ([SOP-013](SOP-013-spark-le-phase-1-feed-spark.md)).
- Phase 2 contracts green on main ([SOP-014](SOP-014-spark-le-phase-2-trust-contracts.md)).

## Procedure

### 1. Batch generation

1. Generate **≤ 8** kit variations per review session (protect ears and judgment).
2. Record `batch_id`, lane, BPM, seeds, `generator_version`.

### 2. SparkLE review session (mandatory)

1. Use [TEMPLATE-spark-le-session-review.md](../templates/TEMPLATE-spark-le-session-review.md).
2. Score **1–5** on: **Sonic**, **Groove**, **Workflow**, **Vibe** (gangsta pressure / phonk dirt / south pocket).
3. **Keeper:** Y/N per kit and per loop stem.
4. **One-line why** — required if any score ≤ 3 or keeper = N.
5. Tag root cause: `sonic` | `groove` | `workflow` | `taste` | `safety`.

### 3. Promote keepers

1. Copy keeper WAVs + manifest to `keepers/<lane>/` (repo or designated store — no secrets).
2. Max **12 keepers per lane**; demote lowest vibe score when full.
3. Never promote clipped or uncleared third-party audio.

### 4. Feed the system

1. **Params:** map tags to `prompt-params` nudges or lane bases (PR with evidence).
2. **RAG:** ingest keeper notes + failure tags ([SOP-007](SOP-007-rag-corpus.md)).
3. **Weekly:** top **3 tags** → exactly **one** engineering task each.

### 5. Business confidence rule

Leadership means **shipping what keepers prove**, not what is loudest:

- Publish / sell / share only kits that pass **keeper bar** or Phase 2 contracts + producer Y.
- If keeper rate drops after a change, **revert first**, innovate second.

## KPIs (Phase 3)

| Metric | Target | Action if miss |
|--------|--------|----------------|
| Keeper rate (kits) | Lane-specific; rise over 4 weeks | Tune DNA/params, not more batch size |
| Sessions with completed review JSON | 100% | Block “silent” generates |
| Repeat failure tag 3× | 0 untriaged | SOP or contract update |

## Success criteria

- [ ] ≥ 4 review sessions logged with template
- [ ] ≥ 3 keepers per active lane
- [ ] Weekly tag → task executed (issue or PR)

## Improvement loop

- Monthly **blind A/B**: two kits same BPM/lane; pick winner; log reason.
- Revise scoring rubric if Groove and Vibe always move together (split definitions in template).

## Escalation

- Customer-facing pack with no keeper provenance → hold release (safety/quality).

**SOP-015 v1.0**
