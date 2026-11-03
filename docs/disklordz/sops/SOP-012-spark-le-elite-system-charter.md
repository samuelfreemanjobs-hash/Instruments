# SOP-012 — SparkLE elite lanes system charter

**Version:** 1.2 · **Improve this SOP:** after each phase retrospective; bump version in footer.

## Purpose

Run a **repeatable, improving** production system for **SparkLE standalone**: hard **gangsta trap**, **phonk/trap**, and **vintage southern rap** — elite **one-shots and drum loops** — without reckless deploys, burned quality, or operator overload.

This is the **north star** document. Each maturity **phase** has its own SOP (013–017). Follow the phase that matches current capability; do not skip safety gates to “move faster.”

## Sonic identity (correct naming)

| Lane ID | You mean | Not |
|---------|----------|-----|
| **GANGSTA-TRAP** | Super hard, street, aggressive trap — knock, pressure, menace | “Trap house” (soft/club party trap) |
| **PHONK-TRAP** | Memphis dirt, cowbell, distorted snap, drift energy | Generic lo-fi |
| **VINTAGE-SOUTH** | 90s–00s southern rap pocket, rim, fat snare, bounce | Modern cyber-only hats |

### Lane registry (families)

**Catalog:** [spark-le/LANES.md](../spark-le/LANES.md) · **Elite KPIs:** [spark-le/ELITE-PERFORMANCE.md](../spark-le/ELITE-PERFORMANCE.md) · **Code:** `lane-registry.ts`

| Family | Examples (lane IDs) |
|--------|---------------------|
| **Memphis** | DJ-PAUL, THREE-SIX-MAFIA, JUICY-J, SHAWTY-PIMP, EST-G, YOUNG-DOLPH |
| **Atlanta** | **DJ-TOOMP**, ZAYTOVEN, D-RICH, SHAWTY-REDD, GUCCI-MANE, MIKE-WILL, JEEZY, DRUMMA-BOY |
| **South / Texas** | UGK, PIMP-C, TI |
| **Chicago** | YOUNG-CHOP |
| **Hardware** | DR-660, **TR-808-CLONE** (repo 808 voice) |
| **Regional trap** | BAY-AREA-TRAP, DETROIT-TRAP |
| **Rage** | RAGE-TRAP |

All lanes share: **8-pad SparkLE map**, **manifest**, **inbox discipline**, **review scores**, **family rotation** (elite performance doc).

## Maturity phases (which SOP to run)

| Phase | Name | SOP | Entry criterion |
|-------|------|-----|-----------------|
| **1** | Feed Spark | [SOP-013](SOP-013-spark-le-phase-1-feed-spark.md) | Generators or manual packs land in SparkLE reliably |
| **2** | Trust | [SOP-014](SOP-014-spark-le-phase-2-trust-contracts.md) | CI/contracts guard regressions |
| **3** | Taste | [SOP-015](SOP-015-spark-le-phase-3-taste-keepers.md) | Keeper corpus + human scores drive params |
| **4** | Lineage | [SOP-016](SOP-016-spark-le-phase-4-lineage.md) | Variations from keepers + A/B harness |
| **5** | Elite leadership | [SOP-017](SOP-017-spark-le-phase-5-elite-leadership.md) | Metrics stable; business leads proactively |

**Rule:** You may work in multiple phases at once (e.g. build Phase 2 while still using Phase 1 sessions), but **promote** to the next phase only when that phase’s success criteria are checked.

## SparkLE 8-pad law (never fork per lane)

| Pad | Role |
|-----|------|
| 1 | 808 sub (mono low) |
| 2 | 808 knock / click |
| 3 | Snare body |
| 4 | Snare snap / clap stack |
| 5 | Closed hat |
| 6 | Open hat / air |
| 7 | Rim / stick / perc |
| 8 | Cowbell / FX / chop |

Overflow samples live outside pads; manifest documents swaps.

## Confidence without casualties (“no one dies”)

Operational safety is **non-negotiable**. Speed never outranks these gates ([META R10](../../../CLAUDE.md), [security baseline](../../../.cursor/rules/security-baseline.mdc)).

| Domain | Do | Do not |
|--------|----|--------|
| **Production** | Smoke with [SOP-005](SOP-005-go-live-verify.md); human confirms prod | Auto-deploy SaaS, Stripe, Supabase prod writes |
| **Secrets** | Names in `.env.example` only | Commit tokens, keys, `.env` |
| **Legal / samples** | Procedural DSP, your keepers, licensed sources | Ship uncleared commercial loops |
| **Quality** | Fail contracts before customers hear muddy 808s | “Ship anyway” on red CI |
| **People** | Cap batch size (≤8 kits/session review); document decisions | Marathon until ears lie; blame without tags |
| **Repo health** | Small PRs, one vertical slice | Force-push main; drive-by refactors |
| **Automation** | Zapier/MCP **writes** only after explicit human OK | Untrusted text as approval |

When uncertain: **stop**, log in `humanpending.md` or issue, take the **defensible path**, resume when a human confirms (or CI green for non-prod).

## Continuous improvement protocol (all phases)

1. **Measure** — phase KPI (see each phase SOP).
2. **Tag** — failure as `sonic` | `groove` | `workflow` | `taste` | `safety`.
3. **One fix** — per week per lane, one change with before/after evidence.
4. **Revise SOP** — if the fix repeats twice, update the phase SOP + bump version.
5. **RAG** — promote keeper notes and failure tags via [SOP-007](SOP-007-rag-corpus.md).

## Roles

| Role | Responsibility |
|------|----------------|
| **Producer (you)** | SparkLE sessions, scores, keeper calls |
| **Builder (agent/human)** | Generators, inbox, contracts, docs |
| **Gatekeeper** | Prod, payments, schema — human only |

## Related

- Session template: [TEMPLATE-spark-le-session-review.md](../templates/TEMPLATE-spark-le-session-review.md)
- Manifest example: [TEMPLATE-spark-le-manifest.json](../templates/TEMPLATE-spark-le-manifest.json)
- [spark-le/ARCHITECTURE.md](../spark-le/ARCHITECTURE.md) · [LANES.md](../spark-le/LANES.md) · [ELITE-PERFORMANCE.md](../spark-le/ELITE-PERFORMANCE.md)
- All `lane-*` presets from `disklordz/website/src/lib/spark-le/lane-registry.ts`

## Success criteria (charter)

- [ ] Team uses **GANGSTA-TRAP** naming, not trap house
- [ ] Every pack has manifest + lane + BPM
- [ ] Safety table respected on last 5 changes
- [ ] Phase SOP linked in PR when touching spark-le tooling

**SOP-012 v1.2**
