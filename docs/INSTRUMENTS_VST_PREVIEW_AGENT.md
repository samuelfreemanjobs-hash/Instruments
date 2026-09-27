# VST preview agent (VS Code / Cursor)

VS Code does not host VSTs. This agent **automates the listen loop**: build → headless WAV clips (JD Upgraded) → files you can play in the editor, plus paths to **Standalone** for live UI.

## Local VS Code / Cursor (build on your machine)

1. **Clone / pull** this repo and open the **root folder** in Cursor or VS Code.
2. Read **[.vscode/README.md](../.vscode/README.md)** (tasks + extensions).
3. **Run Task** → **Instruments: VST preview — local setup** (once per machine).
4. After editing `Source/` or `Wave909/`, **Run Task** → **Instruments: VST preview (git-scoped)**.
5. Open **`vst-testing-ops/previews/latest/*.wav`** (install **Audio Preview** when VS Code suggests it).

**Cursor IDE Agent (local, not Cloud):** start Agent chat with `@instruments-vst-preview-agent` or paste [.vscode/cursor-vst-preview-local.prompt.md](../.vscode/cursor-vst-preview-local.prompt.md).

The rule [`.cursor/rules/instruments-vst-preview.mdc`](../.cursor/rules/instruments-vst-preview.mdc) auto-applies when you edit plugin paths so the agent knows to run previews.

## Quick start (CLI)

```bash
# From repo root (first run configures CMake + builds targets)
python3 vst-testing-ops/preview_agent.py

# After editing Source/ only
python3 vst-testing-ops/preview_agent.py --scope jd --from-git

# Live UI on your machine (not Cloud VM)
python3 vst-testing-ops/preview_agent.py --launch-standalone jd
```

**Output**

- `vst-testing-ops/previews/latest/*.wav` — stable paths for VS Code
- `vst-testing-ops/previews/preview-manifest.json` — run metadata + Standalone paths

In VS Code: install **Audio Preview** (or similar), open a WAV under `previews/latest/`, press play.

## VS Code tasks

Command Palette → **Tasks: Run Task**:

| Task | Action |
|------|--------|
| Instruments: VST preview WAVs | Full preview agent |
| Instruments: VST preview (git-scoped) | Build/render from git diff |
| Instruments: Launch JD Standalone | Interactive JD Upgraded |

## Cursor Cloud Agent

Persona: [.cursor/agents/instruments-vst-preview-agent.md](../.cursor/agents/instruments-vst-preview-agent.md)

Manual workflow dispatch: **Actions → Instruments VST preview**

Local trigger (needs `CURSOR_API_KEY`):

```bash
bash scripts/trigger_vst_preview_agent.sh --dry-run
bash scripts/trigger_vst_preview_agent.sh
```

Optional intake via PM router:

```bash
echo "vst preview after dsp change" | node disklordz/automation/scripts/pm-router-classify.mjs
```

## What each product gets

| Product | In-editor WAV preview | Live UI |
|---------|----------------------|---------|
| **JD Upgraded** | Yes — `OfflineRender` clips from `preview-profile.json` | Standalone binary |
| **Wave909** | No headless WAV yet — `Wave909Tests` smoke only | Standalone / VST3 in DAW |

Clips are defined in [vst-testing-ops/preview-profile.json](../vst-testing-ops/preview-profile.json) (edit to add programs/notes).

## Related

- Full QA: `python3 vst-testing-ops/run_business.py --profile ci`
- Streamlit ops UI: `streamlit run vst-testing-ops/app.py`
- Desktop DAW automation: [BYTEBOT_SETUP.md](BYTEBOT_SETUP.md)
