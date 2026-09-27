# Local VS Code / Cursor — VST preview agent

Use this folder config to **build and hear plugins on your machine** (no Cloud Agent required).

## 1. Get the code

```bash
git fetch origin
git checkout cursor/vst-preview-agent-4170   # or main after PR #67 merges
```

## 2. Open in Cursor (recommended) or VS Code

**File → Open Folder** → repo root (`Instruments`).

Install suggested extension when prompted: **Audio Preview** (plays WAVs in-editor).

## 3. One-time setup

**Terminal → Run Task…** → **Instruments: VST preview — local setup**

Or:

```bash
./scripts/vscode-vst-preview-setup.sh
```

## 4. After you edit plugin C++

| Task | When |
|------|------|
| **Instruments: VST preview (git-scoped)** | Normal loop — builds only what your diff touches |
| **Instruments: VST preview WAVs** | Full build + 3 JD preview clips |
| **Instruments: Launch JD Standalone** | Live UI (no WAV file) |

Preview files: `vst-testing-ops/previews/latest/*.wav`

## 5. Cursor IDE Agent (local)

In **Agent** chat, attach or @-mention:

- `.cursor/agents/instruments-vst-preview-agent.md`

Or paste the starter prompt from `.vscode/cursor-vst-preview-local.prompt.md`.

The agent should run the preview script and give you WAV paths — not Cloud-only workflows.

Docs: [docs/INSTRUMENTS_VST_PREVIEW_AGENT.md](../docs/INSTRUMENTS_VST_PREVIEW_AGENT.md)
