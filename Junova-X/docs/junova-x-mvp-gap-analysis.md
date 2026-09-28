# Junova-X MVP gap analysis

**Date:** 2026-03-17 · **Decisions locked** below.

## Executive summary

**JUCE MVP shipped in monorepo** (`Junova-X/`): VST3 + CLAP + Standalone, Celestial UI, poly DSP, **48 factory presets**, pluginval green. iPlug2 framework is in-repo (`vst-juno106/third_party/iPlug2`); Juno106 product tree still external — [IPLUG2_REFERENCE.md](IPLUG2_REFERENCE.md). **AU:** out of scope. **DAW smoke:** manual checklist in [QA_HOST_SMOKE.md](QA_HOST_SMOKE.md). **Presets:** expand banks post-MVP. **Competitive A/B vs Arturia:** [COMPETITIVE_JUN6.md](COMPETITIVE_JUN6.md).

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
