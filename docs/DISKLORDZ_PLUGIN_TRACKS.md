# Disklordz plugin tracks (Plugin Lab)

**PM:** Airtable · **JUCE factory implementer:** Cursor Cloud Agent · **HISE sketch lane (D):** Antigravity (local Windows) · **Advisory:** Grok Plugin team  

**HISE sketch lane:** [HISE_ANTIGRAVITY_LANE.md](HISE_ANTIGRAVITY_LANE.md) (handoff) · [HISE_SKETCH_LANE.md](HISE_SKETCH_LANE.md) (full) — **Business Planner + Marketing** gate every customer-facing SKU.

## Tracks on `main` today

| Track | Product | Path | Stack | Priority |
|-------|---------|------|-------|----------|
| **A** | **Junova-X** | `Junova-X/` (when merged) | JUCE VST3 + CLAP | **P0** |
| **B** | **TR-808 recreation** (name **TBD**) | [vst-tr808/](../vst-tr808/) | JUCE · WDF/ODE circuit DSP | **P1** — spec on `main`; code after A host green |
| **C** | **JD Upgraded** | `Source/` | JUCE VST3 + CLAP | Maintenance + CI |
| **D** | **HISE sketch** (rompler / sampler SKUs) | [hise-sketch/](../hise-sketch/) | HISE → VST3 (local) | **P3** — Antigravity |

**NovaDrum** was a deprecated placeholder name; use **`[Plugin][TR808]`** and `DL-PLUGIN-TR808` until Marketing locks the final name ([PRODUCT_NAMING.md](../vst-tr808/docs/PRODUCT_NAMING.md)).

## Track D rules

1. Experiments run in parallel with JUCE factory work when **Planner + Marketing** approve.
2. Process row `DL-LANE-HISE-SKETCH` in Airtable is **not** a store product; each shipped SKU gets its own `product_id`.
3. Work order prefix: **`[Plugin][HISE]`** · owner agent: **`antigravity-hise`**.

## WIP (Factory Manager)

Max **2** Cursor implementation WOs on JUCE/repo work. HISE sketches are excluded unless a **JUCE port** work order is opened.
