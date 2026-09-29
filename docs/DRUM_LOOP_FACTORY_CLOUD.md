# Drum Loop Factory OS — Cursor Cloud Agent

The **Drum Loop Factory** Python stack is not the Instruments JUCE plugin tree. Cloud Agents for CCP-3 work should use a dedicated environment and checkout.

## Canonical GitHub repository

| Item | Value |
|------|--------|
| Remote | `https://github.com/samuelfreemanjobs-hash/instruments-Memphis-Drum-Loop-Factory` |
| Local path (this monorepo mirror) | `drum-loop-factory/` |
| Cloud config | `drum-loop-factory/.cursor/environment.json` |

## Point a Cloud Agent at the factory

1. **Push Phases 1–2** from your desktop `drum-loop-factory` project to `instruments-Memphis-Drum-Loop-Factory` on `main` (see `drum-loop-factory/README.md`).
2. In [Cloud Agents → Environments](https://cursor.com/dashboard/cloud-agents/environments), create or edit an environment whose repository is **`samuelfreemanjobs-hash/instruments-Memphis-Drum-Loop-Factory`** (not `Instruments`).
3. Run a **draft environment build** on `main` after `requirements.txt` and `tests/` exist; Save when install succeeds.
4. Start new Cloud Agent runs from that repository when working on `engine/dataset/loader.py` and other CCP-3 modules.

Until the Memphis remote contains your Phase 1–2 tree, you can develop from the copy under `drum-loop-factory/` in this monorepo and push the same commits to the Memphis remote.

## Commands (inside `drum-loop-factory/`)

```bash
pytest tests/ -x -v
ruff check .
```

## Autoclaw

Read-only for Cloud Agents: `.autoclaw/orchestrator/board.md` and `comms/claims/` (when present in the factory repo).
