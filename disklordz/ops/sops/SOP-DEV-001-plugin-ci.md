# SOP-DEV-001 — JUCE plugin CI and pluginval

| Field | Value |
|-------|--------|
| **Owner seat** | hermes-devops |
| **Consumer seats** | hermes-devops, hermes-qa, hermes-dsp |
| **Cadence** | every_plugin_pr |

## Purpose

Plugin changes stay compatible with GitHub **Build** and local `ci-verify`.

## Procedure

1. After C++ edits under `Source/`, `Junova-X/`, `Wave909/`:
   ```bash
   cmake --build build -j
   python3 vst-testing-ops/run_business.py --profile ci-verify
   ```
2. Intentional DSP golden change: `tests/golden/refresh_golden.sh` or Junova refresh script; commit WAVs with explanation.
3. On failure: read `vst-testing-ops/error_log.txt`; fix until green.
4. Junova-X release candidates: also [SOP-DEV-002](SOP-DEV-002-junova-finish-line.md).

## Verification

- Artefacts: JD + Wave909 + Junova VST3 paths in pipeline artefacts stage.
- pluginval passes under xvfb in CI.

## Related

- [docs/REPO_AUTOMATION.md](../../../docs/REPO_AUTOMATION.md)
- [vst-testing-ops/ARCHITECTURE.md](../../../vst-testing-ops/ARCHITECTURE.md)
