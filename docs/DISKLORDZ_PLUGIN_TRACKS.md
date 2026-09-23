# Disklordz plugin tracks (Plugin Lab)

**PM:** Airtable · **JUCE factory implementer:** **juce-factory-elite** Cursor Cloud Agent ([JUCE_FACTORY_AGENT.md](JUCE_FACTORY_AGENT.md)) · **HISE sketch lane (D):** Antigravity (local Windows) · **Advisory:** Grok Plugin team  

**HISE sketch lane:** [HISE_ANTIGRAVITY_LANE.md](HISE_ANTIGRAVITY_LANE.md) (handoff) · [HISE_SKETCH_LANE.md](HISE_SKETCH_LANE.md) (full) — **Business Planner + Marketing** gate every customer-facing SKU.

## Tracks on `main` today

| Track | Product | Path | Stack | Priority |
|-------|---------|------|-------|----------|
| **C** | **JD Upgraded** | `Source/` | JUCE VST3 + CLAP | Maintenance + CI |
| **D** | **HISE sketch** (rompler / sampler SKUs) | [hise-sketch/](../hise-sketch/) | HISE → VST3 (local) | **P3** — Antigravity; does not consume Cursor WIP unless port WO |

Junova-X and NovaDrum (JUCE / iPlug2 factory tracks A–B) are documented on the Junova handoff branch / open PRs — merge those before expecting paths under `Junova-X/` and `vst-tr808/`.

## Track D rules

1. Experiments run in parallel with JUCE factory work when **Planner + Marketing** approve.
2. Process row `DL-LANE-HISE-SKETCH` in Airtable is **not** a store product; each shipped SKU gets its own `product_id`.
3. Work order prefix: **`[Plugin][HISE]`** · owner agent: **`antigravity-hise`**.

## WIP (Factory Manager)

Max **2** Cursor implementation WOs on JUCE/repo work. HISE sketches are excluded unless a **JUCE port** work order is opened.
