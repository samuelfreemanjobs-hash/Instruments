# MeterTap 1.0.0 — release notes

**Released:** 2026-11-09 · **Class:** effect · **Formats:** VST3 (+ Standalone)  
**SOP:** [SOP-VF-08 Release & ship](../../docs/disklordz/sops/SOP-VF-08-release-ship.md)

## What it is

MeterTap is a stereo peak/RMS metering effect with input trim, peak hold, and
bypass. Levels are analysed on the audio thread (factory-lib `PeakRms` +
`MeterBridge`) and displayed through the DiskLordz design-system editor
(`LevelMeter`, `ValueReadout`, `HeaderBar`) at a fixed 480×440 window.

## New in this version

- Design-spec editor (`plugins/MeterTap/.ideas/design-spec.md`) using only
  `vst/factory-lib/gui` components + `factory_gui_use_theme(THEME disklordz)`.
- 30 Hz UI timer: MeterBridge → MeterBallistics → LevelMeter / ValueReadout.
- HOLD / BYPASS / CLEAR / trim bindings per parameter-spec v1.
- Editor offscreen render tests at 100% and 200% for `disklordz` and
  re-theme build `studio-light` (no code change).
- Factory full-lifecycle dogfood path (intake → ship) via
  `disklordz/vst-factory/scripts/e2e_lifecycle.py`.

## Fixed

- Replaced `GenericAudioProcessorEditor` with the design-system surface.
- Scaffold API reuses an existing `vst/MeterTap` tree instead of failing.
- Editor evidence PNGs: truncate before `FileOutputStream` write (no concatenated
  multi-IEND files); normal vs over+bypass renders differ; single IEND at EOF.
- UTF-8 meter title / readouts via `CharPointer_UTF8` (U+00B7 ·, U+2212 −, U+221E ∞).
- RMS wells always show a value (`−∞` at/below −96 dBFS).
- Bypass caution dot: 6 px + 6 px gap; title shifts right (design-spec §4).
- Trim readout always signed (`+0.0 dB`); preset name as-is (`Init`).
- LevelMeter peak/hot split + `formatDb` UTF-8 helpers from design-system
  (`cursor/design-system-juce-impl-1279`).
- Header glyph: simplified floppy+crown vector (`brand.stroke` / `premium.badge`).

## Known issues

- pluginval reports `Num programs: 0` even though `getNumPrograms()` returns 1
  and the program name is `Init`. Treated as a VST3 unit-info / pluginval
  surface quirk; not a host-visible crash. Host preset browser still shows Init.
- Bypass transition click (constitution open issue C) remains a literal instant switch.
- Header glyph is a simplified 22×22 vector (not the full brand SVG asset).

## Compatibility

| Item | Value |
|------|-------|
| OS tested | Linux (Ubuntu Cloud agent, g++-12) |
| DAWs tested (operator) | pluginval strictness 5 (VST3) |
| Sample rates | 44.1 / 48 / 96 kHz (pluginval) |
| Latency | 0 samples |

## Verification evidence

| Check | Result |
|-------|--------|
| JUCE pin | `vst/JUCE` = **7.0.12** (`project(JUCE VERSION 7.0.12)`) |
| `tools/build-vst.sh MeterTap` | exit 0 — VST3 built against JUCE 7.0.12 |
| `run_business.py --profile factory --plugin MeterTap` | `ok: true` (build + artefact + pluginval) |
| `plugins/MeterTap/scripts/verify-phase4.sh` | exit 0 (analysis ctest, processor, editor renders both themes, factory profile) |
| MeterTapEditorRenderTests `disklordz` | PASS @1.0x / @2.0x — single-IEND PNGs, normal≠over |
| MeterTapEditorRenderTests `studio-light` | PASS @1.0x / @2.0x (re-theme configure) |
| Evidence PNGs | `plugins/MeterTap/evidence/editor-renders/{disklordz,studio-light}/` (8 files) |
| Factory e2e (`e2e_lifecycle.py MeterTap`) | all 10 pipeline stages `done` — see `evidence/factory-e2e.json` |
| `tools/apc-gate.sh MeterTap ship` | exit 0 |
| pluginval (Windows) | not run in this Cloud lane |

## Install

See `INSTALL.txt` in the release zip. Font OFL texts for the active theme are
bundled under `licenses/fonts/`.
