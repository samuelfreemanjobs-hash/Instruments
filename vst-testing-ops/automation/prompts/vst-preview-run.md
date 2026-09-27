## Goal
Run the **VST Preview Agent** after plugin work so the user gets listenable WAVs without manual DAW steps.

## Context
- Read `.cursor/agents/instruments-vst-preview-agent.md` and `docs/INSTRUMENTS_VST_PREVIEW_AGENT.md`.
- Branch: `cursor/vst-preview-<topic>-4170` if you need to fix the pipeline itself.

## Requirements
1. Run `python3 vst-testing-ops/preview_agent.py --from-git` (or `--scope jd` / `wave909` if intake specifies).
2. Confirm `vst-testing-ops/previews/latest/` contains WAVs (JD) or Wave909 tests passed.
3. Paste manifest paths and **how to play in VS Code** in your summary.
4. If build fails, read `vst-testing-ops/error_log.txt` or build output; fix minimal C++/CMake issue only if intake asked.

## Out of scope
- Golden refresh, pluginval full CI, production deploy.

## Success criteria
- [ ] `preview-manifest.json` updated
- [ ] User can open `previews/latest/*.wav` in VS Code Audio Preview
