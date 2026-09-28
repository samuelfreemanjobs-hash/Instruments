# SOP-DEV-002 — Junova-X finish line (shippable demo)

| Field | Value |
|-------|--------|
| **Owner seat** | hermes-architect |
| **Consumer seats** | hermes-architect, hermes-dsp, hermes-qa, hermes-gtm |
| **Cadence** | every_junova_release |

## Purpose

Junova-X reaches **Tier C shippable** (demo zip + gates) before marketplace Tier D work.

## Procedure

1. Read [Junova-X/docs/FINISH_LINE.md](../../../Junova-X/docs/FINISH_LINE.md).
2. Build targets: `JunovaX_VST3`, `JunovaX_CLAP`, `JunovaX_Standalone`, `JunovaOfflineRender`, `JunovaXTests`.
3. Run:
   ```bash
   bash Junova-X/scripts/finish_line.sh --mode full
   python3 vst-testing-ops/run_business.py --profile junova-ship
   ```
4. Commit golden WAVs only when DSP change is intentional.
5. hermes-gtm: ensure [Junova-X/gtm/](../../../Junova-X/gtm/) INSTALL and package zip match release.

## Verification

- `vst-testing-ops/reports/junova_finish_line.json` → `"ok": true`.
- CI `junova_finish` stage green.

## Related

- [docs/PRODUCT_FINISH_PLAYBOOK.md](../../../docs/PRODUCT_FINISH_PLAYBOOK.md)
- [SOP-GTM-001](SOP-GTM-001-launch-checklist.md)
