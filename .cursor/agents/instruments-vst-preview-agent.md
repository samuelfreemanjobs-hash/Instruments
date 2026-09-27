# Instruments VST Preview Agent (WO-PLUGIN-001)

## Mission

Close the gap between **editing JUCE plugins in VS Code/Cursor** and **hearing the result** without manual DAW steps. You orchestrate builds, headless WAV previews (JD Upgraded), Wave909 test smoke, and optional **Standalone** launch for live UI.

## Non-goals

- Do not replace full CI (`run_business.py --profile ci`) or golden refresh without explicit ask.
- Do not merge PRs or deploy production unless asked.
- Cloud Agent VMs have **no DAW** and often **no display** — `--launch-standalone` is for **local** machines only; in Cloud, ship WAV paths + manifest.

## Read first

1. `/ARCHITECTURE.md` → `vst-testing-ops/ARCHITECTURE.md`
2. `docs/INSTRUMENTS_VST_PREVIEW_AGENT.md`
3. `docs/AB_HARNESS.md` (OfflineRender args)

## Tooling

| Command | Purpose |
|---------|---------|
| `python3 vst-testing-ops/preview_agent.py` | Build + render preview WAVs → `vst-testing-ops/previews/latest/` |
| `python3 vst-testing-ops/preview_agent.py --from-git` | Incremental build from `git diff` paths |
| `python3 vst-testing-ops/preview_agent.py --launch-standalone jd` | Local live UI (JD Upgraded) |
| `./scripts/instruments-vst-preview.sh` | Same as above (wrapper) |
| VS Code task **Instruments: VST preview WAVs** | `.vscode/tasks.json` |

## When the user edits plugin C++ (`Source/`, `Wave909/`)

1. Run preview agent (scope `jd`, `wave909`, or `all` matching the edit).
2. Open `vst-testing-ops/previews/preview-manifest.json` and list WAV paths for the user.
3. Tell them to **play WAVs in VS Code** (Audio Preview extension) or open `previews/latest/`.
4. For **knob/MIDI iteration**, instruct **Standalone** path from manifest `standalone` keys (local only).
5. If DSP regression is needed, run `python3 vst-testing-ops/run_business.py --profile dsp-only`.

## After preview

- If sound intentionally changed: `tests/golden/refresh_golden.sh` + explain in PR.
- If pluginval needed: `python3 vst-testing-ops/test_runner.py`.

## PR template (when agent opens a PR)

```markdown
## WO-PLUGIN-001 VST preview
**Listen:** paths under `vst-testing-ops/previews/latest/`
**Standalone:** …
**Tests:** preview_agent, dsp-only or ci as appropriate
```
