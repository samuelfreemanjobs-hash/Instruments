# Disklordz plugin tracks (Plugin Lab)

**PM:** Airtable · **JUCE factory implementer:** Cursor Cloud Agent · **HISE sketch lane (D):** Antigravity (local Windows) · **Advisory:** Grok Plugin team  

**HISE sketch lane:** [HISE_ANTIGRAVITY_LANE.md](HISE_ANTIGRAVITY_LANE.md) (handoff) · [HISE_SKETCH_LANE.md](HISE_SKETCH_LANE.md) (full) — **Business Planner + Marketing** gate every customer-facing SKU.

**Three-product rule (808 / Juno family):** [TR808_JUNOVA_PRODUCT_MATRIX.md](TR808_JUNOVA_PRODUCT_MATRIX.md) — **Junova-X**, **NovaDrum** (`vst-tr808/`), and **TR-808 PWA** (`disklordz/tr808-pwa/`) are separate products, branches, and WOs.

## Tracks on `main` today

| Track | Product | Path | Stack | Owner agent |
|-------|---------|------|-------|-------------|
| **C** | **JD Upgraded** | `Source/` | JUCE VST3 + CLAP | Cursor |
| **D** | **HISE sketch** | [hise-sketch/](../hise-sketch/) | HISE → VST3 (local) | Antigravity |
| **E** | **SP-1200 VST** | [SP1200/](../SP1200/) | JUCE (in progress) | **Claude Code** |
| **F** | **Pluggnb Vital preset pack** (1st preset SKU) | [disklordz/packs/vital/](../disklordz/packs/vital/) | Vital `.vital` / `.vitalbank` | **Claude Code** |

## WIP plugin + app tracks (merge pending)

Do **not** lump these in one PR or one `repoPath`. See matrix doc for branch names.

| Track | Product | Path | Stack | Owner agent | Branch (reference) |
|-------|---------|------|-------|-------------|-------------------|
| **A** | **Junova-X** (poly synth) | [Junova-X/](../Junova-X/REPO_HANDOFF.md) | JUCE VST3 + CLAP | Cursor | `cursor/junova-tr808-handoff-94ae` |
| **B** | **NovaDrum** (808-class **plugin**) | [vst-tr808/](../vst-tr808/) | iPlug2 + VST3 (spec) | Cursor / Grok advisory | `cursor/junova-tr808-handoff-94ae`, `cursor/tr808-design-spec-94ae` |
| **App** | **TR-808 PWA** (browser drum machine) | [disklordz/tr808-pwa/](../disklordz/tr808-pwa/ARCHITECTURE.md) | Static PWA + Web Audio | Cursor | `cursor/tr808-pwa-app-94ae` |

**App track (SaaS):** [DISKLORDZ_SAAS_V0.md](DISKLORDZ_SAAS_V0.md) — web WOs stay separate from `[Plugin][Junova-X]` / `[Plugin][NovaDrum]` unless a written cross-product WO says otherwise.

## Track D rules

1. Experiments run in parallel with JUCE factory work when **Planner + Marketing** approve.
2. Process row `DL-LANE-HISE-SKETCH` in Airtable is **not** a store product; each shipped SKU gets its own `product_id`.
3. Work order prefix: **`[Plugin][HISE]`** · owner agent: **`antigravity-hise`**.

## WIP (Factory Manager)

Max **2** Cursor implementation WOs on JUCE/repo work. **Claude Code** owns **Track E** (SP-1200 VST) and **Track F** (Pluggnb Vital preset pack — first Instruments preset SKU) — see [agent-registry.json](agent-registry.json). HISE sketches are excluded unless a **JUCE port** work order is opened.

Priority during Junova push (from handoff): finish **Junova-X** host smoke before NovaDrum **implementation** WOs; NovaDrum spec/DSP docs may run in parallel. **TR-808 PWA** is independent of NovaDrum plugin code.
