# Disklordz plugin tracks (Plugin Lab)

**PM:** Airtable · **JUCE factory implementer:** Cursor Cloud Agent · **HISE sketch lane (D):** Antigravity (local Windows) · **Advisory:** Grok Plugin team  

**App track (SaaS):** [DISKLORDZ_SAAS_V0.md](DISKLORDZ_SAAS_V0.md) — separate lane; do not mix plugin WOs with web WOs in one PR.

**HISE sketch lane:** [HISE_ANTIGRAVITY_LANE.md](HISE_ANTIGRAVITY_LANE.md) (handoff) · [HISE_SKETCH_LANE.md](HISE_SKETCH_LANE.md) (full) — **Business Planner + Marketing** gate every customer-facing SKU.

## Active plugin products

| Track | Product | Path | Stack | Priority |
|-------|---------|------|-------|----------|
| **A** | **Junova-X** | [Junova-X/](../Junova-X/REPO_HANDOFF.md) | **JUCE** VST3 + CLAP | **P0** — JUCE port + host smoke (WO 001–003) |
| **B** | **NovaDrum** (TR-808 class) | [vst-tr808/](../vst-tr808/) | iPlug2 + VST3 | **P1** — Spec/DSP parallel; code after A |
| **C** | **JD Upgraded** | `Source/` | JUCE VST3 + CLAP | Maintenance + CI |
| **D** | **HISE sketch** (rompler / sampler SKUs) | [hise-sketch/](../hise-sketch/) | HISE → VST3 (local) | **P3** — Antigravity; does not consume Cursor WIP unless port WO |

## Priority rule (solo + two Grok teams)

1. **Junova-X** — JUCE scaffold under `Junova-X/` (WO-2026-001 … 003); iPlug2 reference is external / import-only.
2. **TR-808 / NovaDrum:** Grok continues Spec + DSP skeleton **without blocking** Junova JUCE work.
3. **No third plugin** greenfield until one of A or B reaches a ship candidate or is killed in Airtable.

## Track D rules

1. Experiments run in parallel with JUCE factory work when **Planner + Marketing** approve.
2. Process row `DL-LANE-HISE-SKETCH` in Airtable is **not** a store product; each shipped SKU gets its own `product_id`.
3. Work order prefix: **`[Plugin][HISE]`** · owner agent: **`antigravity-hise`**.

## WIP (Factory Manager)

Max **2** Cursor implementation WOs on JUCE/repo work. HISE sketches are excluded unless a **JUCE port** work order is opened.

Example week during Junova push:

- WO-2026-001 `[Plugin][Junova-X]` JUCE VST3 first host green + pluginval smoke
- WO-2026-002 `[Plugin][Junova-X]` CLAP target + dual-format CI checklist
- (Optional) WO-3 `[Plugin][NovaDrum]` voice table only — **docs**, no `plugin/` code

## Grok Plugin team prompt (attach to team)

Use the charter in [GROK_PLUGIN_TEAM.md](GROK_PLUGIN_TEAM.md). Add for this week:

> Continue **Junova-X** from `Junova-X/REPO_HANDOFF.md`. Parallel: **NovaDrum** MVP in `vst-tr808/plugin-spec-mvp.md` — voice schematics for BD→CP first. Do not implement JD Upgraded kernel sharing. Output Airtable-ready WOs with `[Plugin][Junova-X]` or `[Plugin][NovaDrum]` prefixes.

## Landing / GTM

- **Junova-X:** separate `junova-x-landing` repo (Vite, $29/$49).
- Store copy: `Junova-X/gtm/` when added; legacy pointer in `vst-juno106/`.
