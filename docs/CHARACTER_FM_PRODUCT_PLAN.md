# Character FM — product plan (DiskLordz Track E)

**Codename:** Character FM · **Brand module:** CHARACTER ENGINE™  
**Stack:** C++20 · JUCE · Plugin Kernel (future) · VST3 + CLAP + AU (macOS)  
**Status:** Phase 0 — architecture & Milestone 1 spec (no DSP in `main` yet)

## Executive read

This is **not** “another free DX7 clone.” The commercial wedge is:

| Layer | Buyer promise |
|-------|----------------|
| **FM Core** | Familiar 6-op routing, DX7 import/export as a **compatibility layer** |
| **CHARACTER ENGINE** | Living hardware personalities (thermal, power, drift, DNA) — **musically useful** imperfection |
| **Modern FM** | Graph-based algorithms, morphing, macros, morph pad |
| **AI Designer** | Structured preset generate/mutate (never raw SysEx from the model) |
| **Premium UX** | TEMP → POWER → DRIFT → CHARACTER, Character Lab, A/B digital vs aged |

JD Upgraded (Track **C**) stays maintenance + CI. Character FM is a **new kernel product module** once Milestone 1 lands.

## Pillars (scope guard)

Everything ships into one of five pillars — reject orphan features.

1. FM Core  
2. Character Engine  
3. Modern FM  
4. AI Designer  
5. Premium UX  

## MVP (first paid SKU)

See [CHARACTER_FM_ENGINEERING_SPEC.md](CHARACTER_FM_ENGINEERING_SPEC.md) for class layout and Milestone 1 acceptance tests.

**In MVP:** 6 operators, modulation graph, polyphony, APVTS, preset schema v1, Character v1 (drift/thermal/component/phase/DNA + Pristine/Vintage/Chaos), macros, algorithm morph (where safe), DX7 import, AI generate/mutate (local or API behind validator).

**Not in MVP:** cloud accounts, subscription backend, 500 presets, full Living Hardware persistence across sessions (Phase 2), mobile.

## Preset & monetization loop

Your five DX7 presets are **seed Sound DNA**, not the product:

```text
USER PROMPT → AI Designer → validated JSON → native preset
                                    ↓
              CHARACTER DNA (unit lock per preset)
                                    ↓
              CHARACTER LIBRARY SKUs (expansion packs)
```

Ship **Sound DNA** + **Hardware DNA** in every factory preset name (e.g. “Memphis FM · Unit #A7F31 · Vintage”).

## Pricing (draft — Creative Director gate)

| Tier | Price | Includes |
|------|-------|----------|
| Intro | $49 | Synth + core library |
| Standard | $79 | + expanded library + AI credits |
| Producer | $99–129 | + exclusives + expansion bundle |

## Roadmap phases (engineering)

| Phase | Deliverable |
|-------|-------------|
| 0 | Architecture, preset schema, CMake, tests, minimal UI |
| 1 | One operator + MIDI + smoothing |
| 2 | Six operators + voices |
| 3 | Modulation graph + classic algorithm data |
| 4 | Preset browser + versioning |
| 5 | DX7 SysEx in/out via canonical patch |
| 6 | Character Engine v1 + DNA |
| 7 | Modern FM (morph, macros, matrix) |
| 8 | Premium UI |
| 9 | AI Designer + mutator |
| 10 | Commercial QA matrix |

## Agent & factory alignment

- **Product Orchestrator** — family id `character-fm`, compile manifest, Work Orders  
- **Marketing Growth** — positioning, launch copy, CHARACTER LIBRARY naming ([`disklordz/agents/marketing-growth/AGENT.md`](../disklordz/agents/marketing-growth/AGENT.md))  
- **Audio PM** — Airtable WOs, daily Slack rollup target  
- **THOR (Executive Assistant)** — cross-company P0/P1; bridge: [`disklordz/agents/executive-bridge/THOR_TASKMASTER_BRIDGE.md`](../disklordz/agents/executive-bridge/THOR_TASKMASTER_BRIDGE.md)  

## Related docs

- [DISKLORDZ_PLUGIN_TRACKS.md](DISKLORDZ_PLUGIN_TRACKS.md) — Track E  
- [CHARACTER_FM_ENGINEERING_SPEC.md](CHARACTER_FM_ENGINEERING_SPEC.md)  
- [`disklordz/docs/PLUGIN_KERNEL.md`](../disklordz/docs/PLUGIN_KERNEL.md)  
- [`disklordz/AGENTS.md`](../disklordz/AGENTS.md)  
