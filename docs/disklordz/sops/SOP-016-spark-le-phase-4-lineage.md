# SOP-016 — SparkLE Phase 4: Lineage (variation & A/B)

**Phase:** 4 · **Charter:** [SOP-012](SOP-012-spark-le-elite-system-charter.md) · **Version:** 1.0

## Purpose

Elite output **breeds from elite output** — variations from keepers, not random rerolls — with **A/B proof** before replacing production defaults.

## Prerequisites

- Keeper corpus per lane ([SOP-015](SOP-015-spark-le-phase-3-taste-keepers.md)).
- MIDI DNA or generator seeds reproducible ([SOP-014](SOP-014-spark-le-phase-2-trust-contracts.md)).

## Procedure

### 1. Lineage record

For every keeper, store:

- `parent_seed` or `parent_keeper_id`
- `dna_template_id` (hat roll, south bounce, etc.)
- `generator_version`
- Lane + BPM

New batches declare **parent** in manifest `lineage` field.

### 2. Mutate, don’t restart

1. **Wildness** changes: small jitter on params (see generation `wildness` spec).
2. **Groove** changes: DNA template version bump, not only EQ.
3. **Gangsta trap:** mutate knock/clap stack and hat articulation before sub length.

### 3. A/B harness

1. Same BPM/lane: **control** (current default) vs **candidate** (lineage child).
2. Run Phase 2 contracts on both.
3. Producer blind pick + scores in review template.
4. Winner becomes default only after **contracts green + keeper Y or vibe ≥ 4**.

### 4. Retire losers

1. Archive failed candidates with tag (do not delete — learning).
2. Document in PR: what was discarded and why ([META R8](../../../CLAUDE.md) tags).

## KPIs (Phase 4)

| Metric | Target |
|--------|--------|
| % new kits with lineage metadata | → 100% |
| A/B before default swap | 100% |
| Candidate win rate without contract regression | measurable; not 0% |

## Success criteria

- [ ] Default gangsta trap kit traceable to keeper parent
- [ ] One documented A/B with evidence
- [ ] No default swap on single subjective “fire” without contracts

## Improvement loop

- When A/B always picks control: DNA too conservative — increase structured mutation.
- When candidates fail contracts: reduce wildness, not remove contracts.

## Escalation

- Replacing customer-facing default pack → treat like minor release; note in ROADMAP ([SOP-008](SOP-008-roadmap-and-progress.md)).

**SOP-016 v1.0**
