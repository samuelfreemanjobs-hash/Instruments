# Junova-X finish line (shippable product)

This document is the **single definition of done** for taking Junova-X from “in repo” to **marketplace-competitive**. Agents and CI use the same gates — no subjective “feels done.”

## Tiers

| Tier | Name | Meaning | Automation |
|------|------|---------|------------|
| **A** | **Buildable** | Compiles; VST3/CLAP/Standalone + offline render exist | `business_pipeline` artefacts stage |
| **B** | **Trustworthy** | Deterministic DSP; goldens; unit tests; pluginval | `--profile ci-verify` (GitHub **Build**) |
| **C** | **Shippable** | Docs, presets, Linux demo bundle, finish-line report | `Junova-X/scripts/finish_line.sh --mode ci` |
| **D** | **Competitive** | Jun-6 parity matrix green; KR-106 A/B (optional); host smoke evidence | `--mode full` + manual WO items below |

**Today’s goal:** keep **B + C** green on every PR touching `Junova-X/`. **D** closes the gap vs Arturia Jun-6 and free references ([COMPETITIVE_JUN6.md](COMPETITIVE_JUN6.md), [FREE_JUNO_VST_CATALOG.md](FREE_JUNO_VST_CATALOG.md)).

## Tier C gates (finish_line)

| ID | Gate | Script check |
|----|------|----------------|
| C1 | Product architecture doc | `Junova-X/ARCHITECTURE.md` |
| C2 | Competitive + QA docs | `docs/COMPETITIVE_JUN6.md`, `docs/QA_AB_JUN6.md` |
| C3 | ≥ 48 factory presets | `FactoryPresets.cpp` name count |
| C4 | GTM folder + install copy | `gtm/README.md`, `gtm/INSTALL.txt` |
| C5 | Linux demo zip builds | `gtm/package_linux.sh` → `dist/Junova-X-*-linux-x64.zip` |

Tier B is **not** re-run in `--mode ci` (CI already ran `ci-verify`).

## Tier D (marketplace — tracked work orders)

These stay **human or cross-platform** until automated; each must become a WO in Airtable/GitHub:

| ID | Item | Owner seat |
|----|------|------------|
| D1 | Windows signed installer (Inno/MSIX) | architect + qa |
| D2 | `junova-x-landing` repo / Vercel + Stripe | gtm |
| D3 | Preset bank 2 (64+ total story) | presets |
| D4 | Reaper + Ableton host smoke clips ([QA_HOST_SMOKE.md](QA_HOST_SMOKE.md)) | qa |
| D5 | iPlug2 parity table complete ([IPLUG2_REFERENCE.md](IPLUG2_REFERENCE.md)) | dsp |
| D6 | Win32 reference VST captures (Wine sandbox) | qa |
| D7 | KR-106 spectral match with patch-aligned MIDI | dsp |

## Commands

From repo root (after Release build):

```bash
# What CI runs after build (Tier C docs + package)
bash Junova-X/scripts/finish_line.sh --mode ci

# Local release candidate (Tier C + tests + goldens + optional KR-106)
bash Junova-X/scripts/finish_line.sh --mode full

# Monorepo profile (configure + build + ci-verify + finish line)
python3 vst-testing-ops/run_business.py --profile junova-ship
```

## Evidence

- CI: GitHub **Build** job (`ci-verify` + `junova_finish` stage).
- Nightly: **Nightly QA** runs `junova-ship` after full pipeline.
- Report: `vst-testing-ops/reports/junova_finish_line.json` (written by `finish_line.sh`).

## Related

- [ROADMAP.md](ROADMAP.md) — phase status
- [docs/PRODUCT_FINISH_PLAYBOOK.md](../../docs/PRODUCT_FINISH_PLAYBOOK.md) — monorepo-wide “finish products” automation
- [gtm/README.md](../gtm/README.md) — pricing, channels, bundle layout
