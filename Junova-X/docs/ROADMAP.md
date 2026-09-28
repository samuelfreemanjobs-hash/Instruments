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
| 007 | iPlug2 DSP import checklist | **Done** — import path + [PARITY_TABLE.md](PARITY_TABLE.md); iPlug2 product tree still external — [IPLUG2_REFERENCE.md](IPLUG2_REFERENCE.md) |
| 008 | Jun-6 V competitive A/B matrix + golden expansion | **Done** — scenarios, manifest, [QA_AB_JUN6.md](QA_AB_JUN6.md) |
| 009 | IR3109 VCF + chorus BBD v2 | **Done** — OTA tanh VCF + BBD chorus LFO rates; KR-106 smoke compare (WO-2026-012) — [PARITY_TABLE.md](PARITY_TABLE.md) |
| 010 | 6-voice Juno mode | **Done** — voice mode **Juno 6**, UI button, golden `ab-juno6-poly` |
| 011 | Free reference plugins (KR-106 et al.) | **Done** — [REFERENCE_PLUGINS.md](REFERENCE_PLUGINS.md), `setup_kr106_reference.sh`, `compare_kr106_reference.sh` |

## Phase 3 — Product

Automated **Tier C shippable** gates: [FINISH_LINE.md](FINISH_LINE.md) · `bash Junova-X/scripts/finish_line.sh --mode ci`

| ID | Focus | Status |
|----|--------|--------|
| 012 | KR-106 patch-aligned MIDI + smoke A/B | **Done** — fixtures, `compare_kr106_golden.sh`, workflow `junova-kr106-smoke.yml` |
| 013 | Parity table + import path | **Done** — [PARITY_TABLE.md](PARITY_TABLE.md) |
| GTM-001 | `junova-x-landing` scaffold | **Done** — [disklordz/junova-x-landing/](../../disklordz/junova-x-landing/ARCHITECTURE.md), CI `junova-landing.yml` |

- Windows demo installer (Tier D)
- Store Stripe checkout on landing ($29→$49) — human + `hermes-gtm`
- Preset bank 2 (>48 total story)
- Reaper/Reaper-specific QA clips

Monorepo agent loop: [docs/PRODUCT_FINISH_PLAYBOOK.md](../../docs/PRODUCT_FINISH_PLAYBOOK.md)

## Hermes seats

| Seat | Phase 2 ownership |
|------|-------------------|
| architect | This doc + `ARCHITECTURE.md` |
| dsp | `Source/DSP/*`, parity table |
| gui | HPF toggle, arp mode UI, host bar BPM |
| qa | `JunovaXTests`, pluginval, host smoke |
| presets | Bank 2 taxonomy |
