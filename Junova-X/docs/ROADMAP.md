# Junova-X roadmap (post-MVP)

## Phase 1 — Shipped (WO-2026-001 … 003)

- JUCE VST3 + CLAP + Standalone
- Celestial UI + Diag
- 48 factory presets
- pluginval + CI artefact checks

## Phase 2 — Architecture & playability (WO-2026-004+)

| ID | Focus | Status |
|----|--------|--------|
| 004 | Arpeggiator + DSP module split | **Done** — `Arpeggiator`, `BbdChorus`, docs |
| 005 | Host PPQ arp + latch / hold mode | **Done** — PPQ grid 1/16–1/2, `arpLatch`, Celestial host BPM bar |
| 006 | Offline render + golden smoke for Junova | **Done** — `JunovaOfflineRender`, `tests/golden/junova/` |
| 007 | iPlug2 DSP import checklist | **In progress** — submodule + `fetch-deps.sh` + import slot; Juno106 sources external — [IPLUG2_REFERENCE.md](IPLUG2_REFERENCE.md) |
| 008 | Jun-6 V competitive A/B matrix + golden expansion | **Done** — scenarios, manifest, [QA_AB_JUN6.md](QA_AB_JUN6.md) |
| 009 | IR3109 VCF + chorus BBD v2 | **Partial** — OTA tanh VCF + Juno-ish chorus LFO rates (see ARCHITECTURE_DSP) |
| 010 | 6-voice Juno mode | **Done** — voice mode **Juno 6**, UI button, golden `ab-juno6-poly` |

## Phase 3 — Product

- Windows demo installer
- `junova-x-landing` + store ($29→$49)
- Preset bank 2 (>48 total story)
- Reaper/Reaper-specific QA clips

## Hermes seats

| Seat | Phase 2 ownership |
|------|-------------------|
| architect | This doc + `ARCHITECTURE.md` |
| dsp | `Source/DSP/*`, parity table |
| gui | HPF toggle, arp mode UI, host bar BPM |
| qa | `JunovaXTests`, pluginval, host smoke |
| presets | Bank 2 taxonomy |
