# VST testing ops — command center

Plain-English automation for **drop VST3 → run script → AI reads log on failure**. Part of the [Instruments](../ARCHITECTURE.md) monorepo; CI uses the same engine via [`scripts/vst/run_pluginval.py`](../scripts/vst/run_pluginval.py).

## Purpose

Developers and agents validate compiled `.vst3` bundles with **pluginval** (Tracktion) without opening a DAW. Failures land in `error_log.txt` for Cursor / Claude Code to fix C++.

## Layout

| Path | Role |
|------|------|
| `my_plugins/` | Drop finished `.vst3` bundles here (gitignored). |
| `bin/` | Optional local `pluginval` / `pluginval.exe`; otherwise auto-download to `build/tools/pluginval/`. |
| `test_runner.py` | Entry script: strictness 5, in-process validation, writes `error_log.txt` on failure. |
| `error_log.txt` | Generated on failure only (gitignored). |

## Build & run

**Phase 1 — put pluginval in `bin/` (once):**

```bash
python3 vst-testing-ops/test_runner.py --install-pluginval
```

**Phase 2 — drop & run (or auto-find monorepo build artefacts):**

```bash
cmake --build build -j --target JDUpgraded_VST3
python3 vst-testing-ops/test_runner.py

cp -a "build/JDUpgraded_artefacts/Release/VST3/JD Upgraded.vst3" vst-testing-ops/my_plugins/
python3 vst-testing-ops/test_runner.py "JD Upgraded.vst3"
python3 vst-testing-ops/test_runner.py --watch   # re-test on every drop/rebuild copy
```

**Phase 3 — on failure**, open `error_log.txt` in Cursor and ask an agent to fix `Source/` from the log.

Custom plugin name: `VST_NAME=MyFirstPlugin.vst3 python3 vst-testing-ops/test_runner.py`

## Data flow

```
.vst3 in my_plugins/ (or build/*_artefacts)
    → test_runner.py
    → scripts/vst/run_pluginval.py → pluginval CLI
    → pass: exit 0 | fail: error_log.txt
```

## CI

[`.github/workflows/build.yml`](../.github/workflows/build.yml) runs `python3 scripts/vst/run_pluginval.py --default-artefacts` after golden WAV tests.

## Extension points

- Add plugins: drop under `my_plugins/` or extend `resolve_plugin_path()` in `test_runner.py`.
- Stricter checks: pass extra args through by calling `run_pluginval.py` directly (see [tools/ARCHITECTURE.md](../tools/ARCHITECTURE.md)).

## Related docs

- [tools/ARCHITECTURE.md](../tools/ARCHITECTURE.md) — `OfflineRender` / golden WAV DSP regression
- [docs/ARCHITECTURE.md](../docs/ARCHITECTURE.md) — JD Upgraded testing section
