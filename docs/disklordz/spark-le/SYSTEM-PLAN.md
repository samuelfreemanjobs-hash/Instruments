# SparkLE system plan (develop & operate)

**Version:** 1.1 · **Charter:** [SOP-012](../sops/SOP-012-spark-le-elite-system-charter.md) · **Architecture:** [ARCHITECTURE.md](ARCHITECTURE.md)

Living plan for building and running SparkLE elite lanes — ordered work with **done criteria**.

---

## Current state (baseline)

| Layer | Status | Artifact |
|-------|--------|----------|
| Lane catalog | **144** presets | `lane-registry.ts` · `npm run verify:spark-lanes` |
| Prompt → lane | Shipped | `lane-resolve.ts` |
| Audio contracts | **CI** | `audio-contracts.ts` · `npm run verify:spark-audio-contracts` |
| Combined verify | **CI** | `npm run verify:spark` |
| Example manifests | Gangsta, screw, smooth jazz | [examples/](examples/) |
| Keeper SOP | Documented | [KEEPERS.md](KEEPERS.md) |
| 5-session sprint | Ready | [SESSION-SPRINT-5.md](SESSION-SPRINT-5.md) |
| 12-week rotation | Ready | [ELITE-PERFORMANCE.md](ELITE-PERFORMANCE.md) |
| Your keeper WAVs | **You** | `keepers/<presetId>/` per KEEPERS.md |

---

## Phase map

### Phase 1 — Feed Spark ✅ (process ready)

| # | Work item | Status |
|---|-----------|--------|
| 1.1 | Starter manifests (gangsta, screw, smooth jazz) | ✅ |
| 1.2 | 12-week rotation calendar | ✅ |
| 1.3 | First 10 keeper sessions | ⏳ Sprint **3/5 packs complete** ([SPRINT-PROGRESS.md](SPRINT-PROGRESS.md)); keepers pending |

### Phase 2 — Trust ✅ (baseline CI)

| # | Work item | Status |
|---|-----------|--------|
| 2.1 | `verify:spark-lanes` in CI | ✅ |
| 2.2 | Audio contract harness | ✅ `verify:spark-audio-contracts` |
| 2.3 | Tighten thresholds from keepers | ⏳ After Phase 3 data |

### Phase 3 — Taste ⏳

| # | Work item | Status |
|---|-----------|--------|
| 3.1 | Keeper folder convention | ✅ [KEEPERS.md](KEEPERS.md) |
| 3.2 | Blind A/B procedure | ✅ [SOP-016](../sops/SOP-016-spark-le-phase-4-lineage.md) + half-time doc |
| 3.3 | RAG from keeper notes | ⏳ After first 3 keepers |

### Phase 4 — Lineage ⏳

| # | Work item | Status |
|---|-----------|--------|
| 4.1 | Manifest `lineage` in template | ✅ |
| 4.2 | Variation from parent seed in API | ⏳ Future factory field |

### Phase 5 — Leadership ⏳

Follow [SOP-017](../sops/SOP-017-spark-le-phase-5-elite-leadership.md) weekly/monthly/quarterly.

---

## Fast path to top-level (your order)

1. Run **[SESSION-SPRINT-5.md](SESSION-SPRINT-5.md)** — 5 sessions, 5 reviews.
2. Promote **≥3 keepers** → [KEEPERS.md](KEEPERS.md).
3. Weekly: dominant tag → one fix; run `npm run verify:spark`.
4. Week 12 rotation: blind A/B + demote weak keepers.

---

## Sonic pillars (rotation)

1. Hard gangsta trap  
2. Phonk / Memphis / screw  
3. Southern trunk  
4. Smooth & jazz rap  
5. West / G-funk / strip  

---

## Decision log

| Decision | Rationale |
|----------|-----------|
| `verify:spark` = lanes + audio | Single CI gate for Phase 2 |
| Screw LOOP-GRID min 0 | Slow loops fail naive onset counter; tighten when loop-render improves |
| **MPC500 + SparkLE + PC** | MPC = chop/seq foundation; SparkLE = PC param surface; PC = engines — [MPC500-SPARKLE-PC-STACK.md](MPC500-SPARKLE-PC-STACK.md) |
| [808-LAB.md](../808-LAB.md) index | One public name, no second codebase |

---

## Related

[LANE-BACKLOG.md](LANE-BACKLOG.md) · [SUBAGENTS.md](SUBAGENTS.md) · [TRAP-HAT-LOOP-DNA.md](TRAP-HAT-LOOP-DNA.md)
