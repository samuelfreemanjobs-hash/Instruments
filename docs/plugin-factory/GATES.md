# Plugin Factory — quality gates

Copy this file into `<repoPath>/Docs/GATES.md` when scaffolding a product. Update status each run.

**Default factory outcome:** `RELEASE BLOCKED` until Gate 6–7 evidence exists.

Status values: `[ ]` open · `[x]` pass · `[~]` partial · `[!]` blocked

## Gate 1 — Product

- [ ] `plugin.manifest.json` valid against [manifest.schema.json](manifest.schema.json)
- [ ] `pluginName` locked; no alias drift in CMake/UI/docs
- [ ] `Docs/PRODUCT_SPEC.md` matches manifest P0/P1
- [ ] `presetPolicy` vs actual factory preset count documented

## Gate 2 — Architecture

- [ ] `<repoPath>/ARCHITECTURE.md` (build, data flow, threading, modules)
- [ ] `Docs/DSP_SPEC.md` + signal flow
- [ ] `Docs/PARAMETER_MAP.md` stable parameter IDs
- [ ] Root [ARCHITECTURE.md](../../ARCHITECTURE.md) index row added

## Gate 3 — Build

- [ ] Configures on Linux CI compiler (g++-12)
- [ ] Primary format artefacts exist (VST3 minimum)
- [ ] Declared formats in manifest all build or manifest updated

## Gate 4 — DSP

- [ ] Core audio: note → audible output
- [ ] No NaN/Inf in unit/offline smoke
- [ ] Sample-rate / block-size smoke (44.1 kHz + at least one alternate if claimed)

## Gate 5 — Product UX

- [ ] Parameters ↔ UI ↔ automation ↔ serialization aligned
- [ ] Factory presets load; count ≥ `presetPolicy.rcMin` for RC
- [ ] State survives get/set round-trip (automated test)

## Gate 6 — QA automation

- [ ] `ctest` (or product test binary) in CI
- [ ] Offline render determinism OR golden WAV suite
- [ ] State test executable

## Gate 7 — Release

- [ ] `CHANGELOG.md`, user-facing manual stub
- [ ] `Docs/VALIDATION_REPORT.md` with VERIFIED / NOT VERIFIED tables
- [ ] Packaging script + checksums (optional for RC)
- [ ] **NOT VERIFIED:** DAW matrix, pluginval, CPU profile — list explicitly

---

## Release decision

| Milestone | Requires |
|-----------|----------|
| **RC** | Gates 1–6 pass; Gate 7 docs stub OK |
| **Beta** | RC + preset count ≥ `betaMin` + one DAW smoke documented |
| **Retail** | Beta + pluginval clean + `retailMin` presets + CPU note |

Current status line (edit each run):

```text
releaseStatus: blocked | rc | beta | retail
releaseBlockers: (bulleted list)
```
