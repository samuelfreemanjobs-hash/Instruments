# WAVE-909 — quality gates

Manifest: [plugin.manifest.json](../plugin.manifest.json) · Factory gates template: [docs/plugin-factory/GATES.md](../../docs/plugin-factory/GATES.md)

**releaseStatus:** `rc`  
**releaseBlockers:** see manifest `releaseBlockers`

## Gate 1 — Product

- [x] manifest valid
- [x] pluginName locked (WAVE-909)
- [x] PRODUCT_SPEC.md
- [x] 8 factory presets ≥ rcMin (8)

## Gate 2 — Architecture

- [x] ARCHITECTURE.md, DSP_SPEC, PARAMETER_MAP
- [x] Root ARCHITECTURE index

## Gate 3 — Build

- [x] Linux Release build
- [x] VST3, CLAP, Standalone artefacts

## Gate 4 — DSP

- [x] Wave909Tests engine render + filter
- [x] Offline determinism VERIFIED

## Gate 5 — Product UX

- [x] APVTS + UI + presets + state test
- [~] 8 presets < betaMin 24

## Gate 6 — QA automation

- [x] ctest Wave909*
- [x] golden suite (3 WAVs)
- [x] CI in build.yml

## Gate 7 — Release

- [x] CHANGELOG, USER_MANUAL, VALIDATION_REPORT
- [x] package_release.sh
- [!] DAW, pluginval, CPU — NOT VERIFIED

## Release decision

**RC** — yes (Gates 1–6). **Beta** — blocked until betaMin presets + DAW smoke. **Retail** — blocked.
