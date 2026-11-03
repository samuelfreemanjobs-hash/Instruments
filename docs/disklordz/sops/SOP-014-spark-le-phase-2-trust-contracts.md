# SOP-014 — SparkLE Phase 2: Trust (contracts & CI)

**Phase:** 2 · **Charter:** [SOP-012](SOP-012-spark-le-elite-system-charter.md) · **Version:** 1.0

## Purpose

Make quality **objective** so agents and humans lead with confidence: **red CI means stop**, not debate.

## Prerequisites

- Phase 1 feeding Spark reliably ([SOP-013](SOP-013-spark-le-phase-1-feed-spark.md)).
- Generator or render code in repo (sound-factory, website generation, or spark-le scripts).

## Lane contracts (define & test)

Each lane implements the same **contract IDs** with different thresholds:

| Contract ID | What it guards |
|-------------|----------------|
| **808-SUB** | Low-band energy, mono correlation below cutoff |
| **KNOCK** | Transient peak band; decay length |
| **SNARE-STACK** | Body vs snap ratio (lane-specific) |
| **HAT-CLARITY** | High-band mud metric; gangsta trap stricter than phonk |
| **LOOP-GRID** | Onset count / bar for loop stems |
| **SPARK-MANIFEST** | 8 one-shots, required JSON fields, version field |
| **NO-CLIP** | True peak ceiling on exports |

**GANGSTA-TRAP:** tight KNOCK, HAT-CLARITY high, 808-SUB long but controlled.  
**PHONK-TRAP:** SNARE-STACK dirt allowed; HAT-CLARITY relaxed vs gangsta.  
**VINTAGE-SOUTH:** SNARE-STACK body-heavy; LOOP-GRID swing template.

## Procedure

### 1. Before changing DSP or params

1. Identify affected contracts and lanes.
2. Run existing tests / golden audio if present.
3. State success state in PR (not “compiles”).

### 2. Implement contract tests

1. Tests **name the contract** ([META R6](../../../CLAUDE.md)).
2. Fail with **lane + pad + contract ID**.
3. Store fixtures under `tests/` or spark-le fixtures path (when added).

### 3. CI gate

1. PR must pass repo CI ([SOP-001](SOP-001-cloud-agent-task.md)).
2. SparkLE: `cd disklordz/website && npm run verify:spark` (registry + audio/manifest contracts — `audio-contracts.ts`).
3. No merge on contract failure unless `OVERRIDE(R6):` with evidence in PR.

### 4. Version manifests

1. Bump `generator_version` in every manifest when generator changes.
2. Changelog one line per lane in PR description.

### 5. Regression response

1. Reproduce failure locally [executed].
2. Fix or revert — **do not** weaken contract without charter review.

## KPIs (Phase 2)

| Metric | Target |
|--------|--------|
| Contract test pass rate on main | 100% |
| Regressions caught pre-merge | ≥ 1 per month means system working |
| Mystery “sounds off” without tag | → 0 (must map to contract) |

## Success criteria

- [ ] All lane contracts documented with thresholds
- [ ] CI runs contracts on touched generators
- [ ] Last 3 PRs include `generator_version` bump when needed

## Improvement loop

- Quarterly: tighten thresholds **only** if keepers (Phase 3) cluster above old bound.
- Loosen only with tagged evidence + producer sign-off.

## Escalation

- Schema/public API break for SaaS kits → META R10 human confirm.

**SOP-014 v1.0**
