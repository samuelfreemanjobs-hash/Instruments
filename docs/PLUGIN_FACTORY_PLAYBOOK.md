# Plugin Factory OS — playbook (reference)

Extended reference for agents and humans. **Start with the short core:** [PLUGIN_FACTORY_OS.md](PLUGIN_FACTORY_OS.md).

---

## 1. Product intelligence

From the plugin idea, infer:

- Target user and primary workflow  
- Sonic identity (honest, not marketing fiction)  
- Sampleless vs samples (`sampleless` + `userContent` in manifest)  
- Differentiation vs “another generic plugin”  

Output: `Docs/PRODUCT_SPEC.md` (vision, P0/P1, non-goals, sonic/UX philosophy).

---

## 2. DSP architect

Select architecture to match the product — do not add DSP for impressiveness.

Document in `Docs/DSP_SPEC.md`:

- Signal flow diagram  
- Major decisions (DECISION / WHY / ALTERNATIVES / TRADEOFFS / CPU / quality)  
- Anti-aliasing and oversampling policy  
- Voice count and stealing strategy  

---

## 3. Software architecture

Output: `<repoPath>/ARCHITECTURE.md` + `Docs/PARAMETER_MAP.md`.

Cover: threading (audio / message / background), state versioning, extension points, build targets.

Suggested layout:

```text
<repoPath>/
  plugin.manifest.json
  CMakeLists.txt
  Source/ PluginProcessor, Engine/, DSP/, Parameters/, Presets/, UI/
  Tests/ tools/ Docs/ tests/golden/
```

---

## 4. Implementation order (vertical slices)

```text
BUILD SHELL → CORE DSP → PARAMETERS → STATE → PRESETS → UI → FX → TESTS → CI → PACKAGING
```

After each slice: **build, test, fix, continue**.

---

## 5. Parameters

- Stable string IDs; never rename after release without migration  
- Range, default, skew, smoothing per parameter  
- Every new parameter: UI, automation, serialization, presets, tests  

---

## 6. Presets

- Factory only in P0 unless manifest says `sample_based`  
- Categories aligned to product (e.g. BASS, LEAD, BELL for trap synth)  
- Meet `presetPolicy.rcMin` before calling RC  

---

## 7. UI

- Professional instrument — not SaaS/dashboard aesthetic  
- Most important control first; resizable when practical  
- All APVTS parameters either on surface or documented as “advanced/host-only”  

---

## 8. Testing matrix

| Layer | Minimum |
|-------|---------|
| DSP unit | oscillators, filters, envelopes, nonlinear blocks |
| Engine | note on → non-silent buffer |
| State | save/load + program index |
| Offline | deterministic render A/B (`SpectralDiff`) |
| Golden | optional committed WAVs + manifest.tsv |
| Manual | NOT VERIFIED until human/DAW — say so |

Optional: `<repoPath>/test.plan.json` per [test.plan.schema.json](plugin-factory/test.plan.schema.json).

---

## 9. CI (this monorepo)

- Extend [.github/workflows/build.yml](../.github/workflows/build.yml) with product artefact checks  
- `ctest` for product tests  
- Golden script in product tree  

---

## 10. Release artefacts

```text
Release/ or Scripts/package_release.sh
  Plugin binaries (VST3, CLAP, …)
  README, CHANGELOG, USER_MANUAL
  VALIDATION_REPORT.md
  SHA256SUMS
```

---

## 11. Multi-agent roles (optional)

| Role | Owns |
|------|------|
| Product | manifest, PRODUCT_SPEC |
| DSP | DSP_SPEC, algorithms |
| C++ | Source, CMake |
| UI | Editor, LookAndFeel |
| QA | tests, VALIDATION_REPORT, GATES |
| Release | packaging, CHANGELOG |

Communication via **git files**, not chat-only context.

---

## 12. Version migration

When state or parameter IDs change: detect version in saved state, migrate or document break.

---

## 13. Security

Untrusted preset paths; no directory traversal; no arbitrary code from preset files.

---

## 14. Factory status report (each milestone)

```text
PROJECT: (pluginName from manifest)
STAGE: (stage machine)
STATUS: IN PROGRESS | PASS | BLOCKED
COMPLETED / VERIFIED / NOT VERIFIED / ISSUES / NEXT
```

Final line: **RELEASE BLOCKED** or **RC / beta / retail** with gate reference.

---

## 15. v2 manifest pipeline (target)

```text
plugin.manifest.json
  → product.spec.json (optional extract)
  → dsp.spec.json
  → parameter.schema.json
  → test.plan.json
  → build.config.json (CMake target names)
```

WAVE-909 reference: [Wave909/plugin.manifest.json](../Wave909/plugin.manifest.json).
