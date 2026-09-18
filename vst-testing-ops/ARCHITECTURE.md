# VST testing ops — command center

Operations hub for the **Instruments** monorepo: one pipeline matches [`.github/workflows/build.yml`](../.github/workflows/build.yml), optional Disklordz SaaS smoke, Streamlit UI, and single-plugin fast lanes.

## Purpose

Run the whole product QA loop without manual DAW checks — configure/build, artefact gates, DSP determinism, golden WAV regression, Wave909 unit tests, pluginval, and (optionally) `disklordz/website` + daw-inbox syntax checks.

## Layout

| Path | Role |
|------|------|
| `business_pipeline.py` | Stage runners + profiles (`ci`, `full`, `plugin-quick`, `dsp-only`). |
| `run_business.py` | CLI: `python3 vst-testing-ops/run_business.py --profile ci` |
| `app.py` | Streamlit **Operations Command Center** (fleet status + full pipeline). |
| `test_runner.py` | Single-VST pluginval + `error_log.txt` / `--watch`. |
| `my_plugins/` | Drop `.vst3` bundles (gitignored). |
| `bin/` | Optional local `pluginval` binary. |
| `reports/` | Timestamped JSON run history (gitignored). |
| `error_log.txt` | First failing stage tail for agents (gitignored). |
| `last_run_report.json` | Latest pipeline summary (gitignored). |

## Pipeline stages (profile `ci`)

1. **configure** — CMake Release (`g++-12` on Linux)
2. **build** — full monorepo `cmake --build build -j`
3. **artefacts** — JD Upgraded VST3, CLAP, Standalone exist
4. **determinism** — dual `OfflineRender` + `SpectralDiff`
5. **golden** — `tests/golden/verify_golden.sh`
6. **wave909_tests** — `Wave909Tests` or `ctest -R Wave909`
7. **pluginval** — `scripts/vst/run_pluginval.py --default-artefacts`

Profile **`full`** adds **disklordz_web** (`npm ci` / `npm run build` + daw-inbox `node --check`).

## Build & run

**Whole business (CLI, CI parity):**

```bash
python3 vst-testing-ops/run_business.py --profile ci
python3 vst-testing-ops/run_business.py --profile full    # + SaaS
python3 vst-testing-ops/run_business.py --profile ci --with-saas
```

**Dashboard:**

```bash
pip install -r vst-testing-ops/requirements.txt
streamlit run vst-testing-ops/app.py
```

**Single-plugin stress only:**

```bash
python3 vst-testing-ops/test_runner.py --install-pluginval
python3 vst-testing-ops/test_runner.py --watch
```

## Data flow

```
run_business.py / app.py
  → business_pipeline.run_pipeline
  → stages (subprocess): cmake, OfflineRender, golden, ctest, run_pluginval.py
  → pass: last_run_report.json | fail: error_log.txt + report
```

## CI

GitHub Actions runs the same entrypoint as local agents:

[`python3 vst-testing-ops/run_business.py --profile ci`](../vst-testing-ops/run_business.py) in [`.github/workflows/build.yml`](../.github/workflows/build.yml).

## Automation roadmap

| Layer | Status | Next step |
|-------|--------|-----------|
| **Gate on every PR** | `build.yml` → `run_business.py --profile ci` | Require check before merge; optional Slack on fail (`ci-slack-notify.yml`) |
| **Local / agent habit** | `AGENTS.md` commands | Cloud Agent: run `--profile ci` after plugin DSP changes |
| **Post-compile watch** | `test_runner.py --watch my_plugins/` | Optional `entr`/`watchmedo` on `build/*_artefacts` in dev |
| **Full business** | `--profile full` | Run nightly or pre-release (Disklordz + plugins) |
| **VST3 DSP via bundle** | Not built | Future: headless host MIDI→WAV through `.vst3` (complement `OfflineRender`) |
| **Dashboard** | Streamlit `app.py` | Optional long-running service on dev machine only |

**Cursor should:** after C++ plugin edits, run `python3 vst-testing-ops/run_business.py --profile ci` (or `dsp-only` for DSP-only diffs); on failure read `vst-testing-ops/error_log.txt`, patch, re-run until green; update golden WAVs only via `tests/golden/refresh_golden.sh` when DSP output intentionally changes.


## Related docs

- [tools/ARCHITECTURE.md](../tools/ARCHITECTURE.md) — offline tools
- [docs/ARCHITECTURE.md](../docs/ARCHITECTURE.md) — JD Upgraded testing
- [disklordz/website/ARCHITECTURE.md](../disklordz/website/ARCHITECTURE.md) — SaaS build
