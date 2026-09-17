# Junova-X MVP gap analysis

**Date:** 2026-03-17 · **Decisions locked** below.

## Executive summary

Junova-X has a **real iPlug2 scaffold** (DSP, Main+Diag, VST3 project, QA docs) but **zero DAW verification**. Disklordz standard is **JUCE + VST3/CLAP**. **Decision: port to JUCE** under `Junova-X/` — iPlug2 remains reference only. **AU:** out of scope (no Apple). **CLAP:** MVP-hard (WO-2026-002). **Presets:** **48** factory for MVP; **more banks later**.

## Gap table

| Area | Reference (iPlug2) | MVP target (JUCE) | WO |
|------|-------------------|-------------------|-----|
| Stack | iPlug2 + VST3 | JUCE VST3 + CLAP | 001, 002 |
| Repo path | external / `plugin/Juno106/` | `Junova-X/` monorepo | 001 |
| Host proof | None | Reaper + pluginval | 001 |
| CLAP | N/A | CLAP host smoke | 002 |
| CI | None | Instruments CMake + docs | 002 |
| Presets | TBD | **48** factory + user save | 003 |
| JD kernel | — | **No share** | — |
| Windows demo | GTM | After 001/002 | release |
| SaaS / App team | — | Separate lane | — |

## Ship risks

1. **Port fidelity** — DSP/UI parity iPlug2 → JUCE (schedule explicit checklist in 001).
2. **CLAP** — second format before revenue (accepted for Disklordz default).
3. **Solo bandwidth** — max 2 active WOs; Junova blocks NovaDrum code.

## Work orders (Airtable)

| ID | Title |
|----|--------|
| WO-2026-001 | [Plugin][Junova-X] JUCE VST3 first host green + pluginval smoke |
| WO-2026-002 | [Plugin][Junova-X] CLAP target + dual-format CI checklist |
| WO-2026-003 | [Plugin][Junova-X] Factory presets (48 MVP bank) + state save/load |

Seed file: `disklordz/airtable/seed/work-orders-junova-2026.json`  
Import: `python3 disklordz/automation/scripts/seed_work_orders.py`
