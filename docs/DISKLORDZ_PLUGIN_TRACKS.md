# Disklordz plugin tracks (Plugin Lab)

**PM:** Airtable · **JUCE factory implementer:** Cursor Cloud Agent · **HISE sketch lane (D):** Antigravity (local Windows) · **Advisory:** Grok Plugin team  

**Stack policy:** **JUCE** is the ship stack for all Plugin Lab native products (including NovaDrum). iPlug2 trees are import/reference only.

**Three-product rule:** [TR808_JUNOVA_PRODUCT_MATRIX.md](TR808_JUNOVA_PRODUCT_MATRIX.md) — **Junova-X**, **NovaDrum**, and **TR-808 PWA** are separate products, **branches**, and PRs.

## Tracks on `main` today

| Track | Product | Path | Stack | Owner agent |
|-------|---------|------|-------|-------------|
| **C** | **JD Upgraded** | `Source/` | JUCE VST3 + CLAP | Cursor |
| **D** | **HISE sketch** | [hise-sketch/](../hise-sketch/) | HISE → VST3 (local) | Antigravity |
| **E** | **SP-1200 VST** | [SP1200/](../SP1200/) | JUCE (in progress) | **Claude Code** |
| **F** | **Pluggnb Vital preset pack** (1st preset SKU) | [disklordz/packs/vital/](../disklordz/packs/vital/) | Vital `.vital` / `.vitalbank` | **Claude Code** |
| **G** | **WAVE-909** | [Wave909/](../Wave909/) | JUCE VST3 + CLAP + Standalone | Cursor |

**G** is the Plugin Factory **reference** implementation ([PLUGIN_FACTORY_OS.md](PLUGIN_FACTORY_OS.md)); new JUCE SKUs copy its manifest/CI pattern.

## WIP tracks (merge pending — one branch each)

| Track | Product | Path | Stack | Branch |
|-------|---------|------|-------|--------|
| **A** | **Junova-X** (+ legacy `vst-juno106/` import) | [Junova-X/](../Junova-X/REPO_HANDOFF.md) | JUCE VST3 + CLAP | **`cursor/junova-x-handoff-029a`** |
| **B** | **NovaDrum** | [vst-tr808/](../vst-tr808/REPO_HANDOFF.md) | JUCE VST3 + CLAP | **`cursor/novadrum-juce-handoff-029a`** |
| **P** | **TR-808 PWA** (standalone app lane) | [disklordz/tr808-pwa/](../disklordz/tr808-pwa/ARCHITECTURE.md) | Static PWA + Web Audio | **`cursor/tr808-pwa-app-94ae`** |

Do **not** use `cursor/junova-tr808-handoff-94ae` for new work (legacy combined handoff).

**Disklordz SaaS** (kit generator): [DISKLORDZ_SAAS_V0.md](DISKLORDZ_SAAS_V0.md) — **not** track **P**; no shared PR with PWA unless a written cross-product WO exists.

## Track D rules

1. Experiments run in parallel with JUCE factory work when **Planner + Marketing** approve.
2. Process row `DL-LANE-HISE-SKETCH` in Airtable is **not** a store product; each shipped SKU gets its own `product_id`.
3. Work order prefix: **`[Plugin][HISE]`** · owner agent: **`antigravity-hise`**.

## WIP (Factory Manager)

Max **2** Cursor implementation WOs on JUCE/repo work. **Claude Code** owns **E** and **F** — see [agent-registry.json](agent-registry.json). HISE sketches excluded unless **JUCE port** WO.

Priority: Junova-X host smoke before NovaDrum **implementation** WOs; NovaDrum spec may run in parallel. **Track P (PWA)** is independent.
