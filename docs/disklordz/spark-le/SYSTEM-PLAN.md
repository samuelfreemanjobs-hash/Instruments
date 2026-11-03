# SparkLE system plan (develop & operate)

**Version:** 1.0 · **Charter:** [SOP-012](../sops/SOP-012-spark-le-elite-system-charter.md) · **Architecture:** [ARCHITECTURE.md](ARCHITECTURE.md)

This is the **living plan** for building and running the SparkLE elite lane system — not a calendar timeline, but ordered work with clear done criteria.

---

## Current state (baseline)

| Layer | Status | Artifact |
|-------|--------|----------|
| Lane catalog | **141** presets (`npm run verify:spark-lanes`) | `lane-registry.ts` |
| Prompt → lane | Neutral preset + brief infers lane | `lane-resolve.ts` |
| Generation | Drum params → factory WAV + manifest | `prompt-params.ts`, `factory.ts` |
| Docs / SOPs | Phases 1–5, DNA, index, half-time | `docs/disklordz/sops/SOP-013`–`017` |
| Memory | Review template, weekly CI doc | [MEMORY-AND-CI.md](MEMORY-AND-CI.md) |
| Audio contracts | Spec in SOP-014; **automated WAV tests pending** | — |

---

## Phase map (what to build next)

### Now — Phase 1 hardening (Feed Spark)

**Goal:** Every session ends with loadable SparkLE assets + one review row.

| # | Work item | Done when |
|---|-----------|-----------|
| 1.1 | **Starter pack manifest** per flagship lane (8 pads + 2-bar top stem path) | [examples/manifest-smooth-jazz-rap.json](examples/manifest-smooth-jazz-rap.json) + base [template](../templates/TEMPLATE-spark-le-manifest.json) |
| 1.2 | **Family rotation calendar** (12 weeks) in ELITE-PERFORMANCE | Each family has assigned week + KPI |
| 1.3 | **First 10 keeper sessions** logged with tags | Reviews in your store; at least 3 lanes with keeper Y |

### Next — Phase 2 (Trust / contracts)

**Goal:** Regressions caught before merge.

| # | Work item | Done when |
|---|-----------|-----------|
| 2.1 | **`verify:spark-lanes` in CI** on `disklordz/website` path changes | GitHub workflow step green |
| 2.2 | **Contract test harness** for generated WAV (peak, mono sub, hat band) | Script fails on bad fixture; passes on golden kit |
| 2.3 | Map contracts → lanes in SOP-014 threshold table | 808-SUB, HAT-CLARITY rows per family |

### Then — Phase 3 (Taste / keepers)

**Goal:** Params move only with evidence.

| # | Work item | Done when |
|---|-----------|-----------|
| 3.1 | Keeper folder convention `keepers/<presetId>/` documented + one example | README in template or MEMORY-AND-CI |
| 3.2 | **Blind A/B** procedure default for param changes | SOP-016 checklist used once |
| 3.3 | RAG snippets fed from top 3 keeper notes | `knowledge-snippets.ts` PR with [executed] |

### Later — Phase 4 (Lineage)

| # | Work item | Done when |
|---|-----------|-----------|
| 4.1 | Manifest `lineage` + `dna_template_id` required on keeper promote | Schema bump in template |
| 4.2 | Variation batch from keeper parent seed | factory batch API uses parent seed field |

### Steady — Phase 5 (Leadership)

Follow [SOP-017](../sops/SOP-017-spark-le-phase-5-elite-leadership.md) weekly/monthly/quarterly cadence.

---

## Your sonic pillars (rotation priority)

Use these when choosing **which lane to session** each week:

1. **Hard gangsta trap** — Jeezy / Mike Will / trap-or-die family  
2. **Phonk / Memphis / screw** — OG phonk, slow drag, DJ Screw, slowed R&B  
3. **Southern trunk** — Houston eras, NOLA bounce, UGK pocket  
4. **Smooth & jazz rap** — `lane-smooth-jazz-rap`, Curren$y, Knxwledge, neo-soul, lo-fi jazz trap  
5. **West / G-funk / strip** — DPG, Snoop, Dre Day, strip anthem  

---

## Decision log (fill as you go)

| Date | Decision | Rationale |
|------|----------|-----------|
| — | Generic **smooth jazz rap** lane vs Curren$y-only tags | Broader brief inference without losing artist lane |
| — | Prompt inference only on neutral factory presets | Explicit lane choice stays authoritative |

---

## Related

- [LANE-BACKLOG.md](LANE-BACKLOG.md) · [SUBAGENTS.md](SUBAGENTS.md) · [TRAP-HAT-LOOP-DNA.md](TRAP-HAT-LOOP-DNA.md)
