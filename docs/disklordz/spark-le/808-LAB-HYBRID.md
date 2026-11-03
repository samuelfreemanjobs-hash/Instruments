# DiskLordz 808 Lab ↔ SparkLE hybrid (architecture review)

**Version:** 1.0 · **Status:** Recommended **single product surface** — not a second parallel app.

You asked whether a **SparkLE hybrid system** already covers (or beats) a standalone **DiskLordz 808 Lab**. **Yes — if “808 Lab” means your end-to-end 808/sub/knock/screw/southern workflow.** The hybrid stack is the Lab; SparkLE is the **hands-on bench**, Disklordz generation is the **factory floor**.

---

## What “808 Lab” usually means (your world)

| Need | Your music |
|------|------------|
| Sub + knock design | Gangsta trap, Houston screw, southern trunk |
| Lane/taste memory | 144 archetype presets + keepers |
| Hardware truth | TR-808 clone lane, DR-660, TR-909, classic electro 808 |
| Standalone speed | SparkLE 8-pad law, no DAW |
| Quality gates | Contracts, CI, reviews |
| Continuous improvement | Tags → registry/DNA/RAG |

That is **exactly** what the hybrid system is built to do — not a separate SKU unless you want a branded UI shell on top.

---

## Hybrid stack (already in repo)

```text
Brief / lane / RAG suggest
  → coalesceSparkPreset (lane-resolve.ts)
  → resolveDrumParams + factory WAV (website) | sound-factory (Python)
  → verify:spark (registry + audio-contracts + manifests)
  → manifest (examples/ + TEMPLATE) → SparkLE load
  → SESSION-SPRINT / review / keepers/
  → weekly tag → lane-registry | TRAP-HAT-LOOP-DNA | knowledge-snippets
```

| Layer | Path | Role in “808 Lab” |
|-------|------|-------------------|
| **Lane brain** | `lane-registry.ts` | Every 808 personality (screw, trap, smooth, electro…) |
| **Prompt → preset** | `lane-resolve.ts` | “fat pat screw” without clicking 144 lanes |
| **Trust** | `audio-contracts.ts`, `npm run verify:spark` | Objective sub/knock/hat/manifest gates |
| **SparkLE feed** | SOP-013, manifests, daw-inbox (optional) | Pad 1–8 law |
| **Taste** | KEEPERS.md, SOP-015 | Your ear → durable defaults |
| **Lineage** | SOP-016, manifest `lineage` | Mutations from keepers |
| **Leadership** | SOP-017, 12-week rotation | No drift across families |
| **RAG** | `knowledge-snippets.ts`, `/api/rag/suggest` | Brief coaching |
| **Hardware alignment** | `lane-tr-808-clone` + repo SP-1200 / plugin docs | Clone voice target (calibration loop still manual) |

**Factory presets that are “808 Lab neutral bench”:** `mpc-ready-808`, `disklordz-screw`, `lane-tr-808-clone`.

---

## Would hybrid cover *everything* here?

| Involved piece | Hybrid covers? | Notes |
|----------------|----------------|-------|
| Gangsta / trap 808 | ✅ | Lanes + Session 1 manifest + KNOCK/808-SUB contracts |
| Screw / slowed R&B | ✅ | Session 2 manifest, `lane-dj-screw`, `lane-slowed-rnb` |
| Smooth / jazz rap sub | ✅ | Softer decay lanes; different hat DNA — still 808-style factory |
| 144 style lanes | ✅ | Registry + inference |
| SparkLE standalone | ✅ | Charter pad map |
| SaaS kit generation | ✅ | `disklordz/website` factory |
| CI / no regressions | ✅ | `verify:spark` in website workflow |
| Keeper memory | ✅ Process | **Your** `keepers/` fills Phase 3 |
| Live plugin 808 synth | ⚠️ Partial | **Wave9090** / JD Upgraded are **synth** lanes — not the same as drum one-shots; link optional |
| Auto-drop into SparkLE app | ⚠️ Gap | daw-inbox exists; OS-specific SparkLE import still manual |
| Per-lane golden WAV suite | ⚠️ Gap | Contracts use synthetic render; golden refs like VST ops not yet per lane |
| TR-808 clone DSP = lane params | ⚠️ Gap | Documented intent; automated sync from C++ plugin to `lane-tr-808-clone` not wired |

So: **conceptually the hybrid is the 808 Lab.** Separate “808 Lab” app only pays off if you need a **dedicated UI** (visual sub analyzer, pad tuner, A/B player) — backend should still be this stack to avoid two truths.

---

## Handle it *better* than a siloed Lab?

**Hybrid wins when:**

1. One **manifest** format → SparkLE + store + lineage.
2. One **registry** → SaaS + RAG + reviews reference same `presetId`.
3. **CI** blocks bad 808 exports before they touch your ears.
4. **Sprint + keepers** compound; a standalone Lab without Phase 3–5 becomes a sample folder.

**A siloed 808 Lab only wins if:**

- It adds **measurement** UI (spectrum, decay meters, mono correlation) on every export — we should **add that to the hybrid** as `808-LAB-TOOLS` (future script or small web panel), not fork the catalog.

---

## Recommended naming (one brand, one pipeline)

| Public name | Internal |
|-------------|----------|
| **DiskLordz 808 Lab** | Product/marketing umbrella for you + packs |
| **SparkLE hybrid** | Operating model: factory + standalone + SOPs |
| **SparkLE elite lanes** | `lane-registry.ts` catalog |

Add to pack copy: *“Built in DiskLordz 808 Lab — SparkLE-ready manifest included.”*

---

## Next engineering (808 Lab gaps, not duplicate apps)

1. **Golden kit per flagship lane** (trap-or-die, dj-screw, smooth-jazz-rap) under `disklordz/website/tests/spark-fixtures/` for contract drift.
2. **Inbox → SparkLE** smoke doc + watch path for your OS ([daw-inbox/ARCHITECTURE.md](../../../disklordz/daw-inbox/ARCHITECTURE.md)).
3. **808 analyzer script** (mono sub ratio, decay time) reusing `audio-contracts.ts` on **your** exported WAVs post-session.
4. **TR-808 clone alignment** session: SP-1200/clone plugin A/B vs `lane-tr-808-clone` one-shots.

---

## Related

[SYSTEM-PLAN.md](SYSTEM-PLAN.md) · [ARCHITECTURE.md](ARCHITECTURE.md) · [SOP-012](../sops/SOP-012-spark-le-elite-system-charter.md)
