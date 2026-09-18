# Disklordz plugin tracks (Plugin Lab)

**PM:** Airtable · **JUCE factory implementer:** Cursor Cloud Agent · **HISE sketch lane (D):** Antigravity (local Windows) · **Advisory:** Grok Plugin team  

**HISE sketch lane:** [HISE_ANTIGRAVITY_LANE.md](HISE_ANTIGRAVITY_LANE.md) (handoff) · [HISE_SKETCH_LANE.md](HISE_SKETCH_LANE.md) (full) — **Business Planner + Marketing** gate every customer-facing SKU.

## Tracks on `main` today

| Track | Product | Path | Stack | Owner agent |
|-------|---------|------|-------|-------------|
| **C** | **JD Upgraded** | `Source/` | JUCE VST3 + CLAP | Cursor |
| **D** | **HISE sketch** | [hise-sketch/](../hise-sketch/) | HISE → VST3 (local) | Antigravity |
| **E** | **SP-1200 VST** | [SP1200/](../SP1200/) | JUCE (in progress) | **Claude Code** |
| **F** | **Pluggnb Vital preset pack** (1st preset SKU) | [disklordz/packs/vital/](../disklordz/packs/vital/) | Vital `.vital` / `.vitalbank` | **Claude Code** |

Junova-X and NovaDrum (JUCE / iPlug2 factory tracks A–B) are documented on the Junova handoff branch / open PRs — merge those before expecting paths under `Junova-X/` and `vst-tr808/`.

## Track D rules

1. Experiments run in parallel with JUCE factory work when **Planner + Marketing** approve.
2. Process row `DL-LANE-HISE-SKETCH` in Airtable is **not** a store product; each shipped SKU gets its own `product_id`.
3. Work order prefix: **`[Plugin][HISE]`** · owner agent: **`antigravity-hise`**.

## WIP (Factory Manager)

Max **2** Cursor implementation WOs on JUCE/repo work. **Claude Code** owns **Track E** (SP-1200 VST) and **Track F** (Pluggnb Vital preset pack — first Instruments preset SKU) — see [agent-registry.json](agent-registry.json). HISE sketches are excluded unless a **JUCE port** work order is opened.
