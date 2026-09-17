# Disklordz plugin tracks (Plugin Lab)

**PM:** Airtable · **JUCE factory implementer:** Cursor Cloud Agent · **HISE sketch lane (D):** Antigravity (local Windows) · **Advisory:** Grok Plugin team  

**HISE sketch lane:** [HISE_ANTIGRAVITY_LANE.md](HISE_ANTIGRAVITY_LANE.md) (handoff) · [HISE_SKETCH_LANE.md](HISE_SKETCH_LANE.md) (full) — **Business Planner + Marketing** gate every customer-facing SKU.

## Tracks on `main` today

| Track | Product | Path | Stack | Priority |
|-------|---------|------|-------|----------|
| **C** | **JD Upgraded** | `Source/` | JUCE VST3 + CLAP | Maintenance + CI |
| **D** | **HISE sketch** (rompler / sampler SKUs) | [hise-sketch/](../hise-sketch/) | HISE → VST3 (local) | **P3** — Antigravity; does not consume Cursor WIP unless port WO |
| **E** | **Character FM** (6-op FM + CHARACTER ENGINE) | `products/CharacterFM/` (planned) · spec in [CHARACTER_FM_ENGINEERING_SPEC.md](CHARACTER_FM_ENGINEERING_SPEC.md) | JUCE VST3 + CLAP | **P1** — Milestone 1 after Orchestrator + Marketing gate |

Junova-X and NovaDrum (JUCE / iPlug2 factory tracks A–B) are documented on the Junova handoff branch / open PRs — merge those before expecting paths under `Junova-X/` and `vst-tr808/`.

## Track D rules

1. Experiments run in parallel with JUCE factory work when **Planner + Marketing** approve.
2. Process row `DL-LANE-HISE-SKETCH` in Airtable is **not** a store product; each shipped SKU gets its own `product_id`.
3. Work order prefix: **`[Plugin][HISE]`** · owner agent: **`antigravity-hise`**.

## WIP (Factory Manager)

Max **2** Cursor implementation WOs on JUCE/repo work. HISE sketches are excluded unless a **JUCE port** work order is opened. **Track E** (Character FM) counts toward the same WIP cap — coordinate with Audio PM before starting M1 alongside large JD changes.

## Track E docs

- [CHARACTER_FM_PRODUCT_PLAN.md](CHARACTER_FM_PRODUCT_PLAN.md)
- [CHARACTER_FM_ENGINEERING_SPEC.md](CHARACTER_FM_ENGINEERING_SPEC.md)
- Agent bridge: [`disklordz/agents/executive-bridge/THOR_TASKMASTER_BRIDGE.md`](../disklordz/agents/executive-bridge/THOR_TASKMASTER_BRIDGE.md)
