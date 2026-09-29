# instruments-Memphis-Drum-Loop-Factory

DDSP agent and ML pipeline for 1990s Memphis phonk drum loops (**Drum Loop Factory OS**).

This repository is the **primary checkout** for Cursor Cloud Agents working on the factory (not the Instruments JUCE monorepo).

## Local → GitHub (required once)

Your Phases 1–2 work may still live only on disk. Push it here so Cloud Agents can run `pytest` and implement CCP-3 modules:

```bash
cd /path/to/drum-loop-factory   # e.g. Desktop/Projects/drum-loop-factory
git remote add origin https://github.com/samuelfreemanjobs-hash/instruments-Memphis-Drum-Loop-Factory.git
git branch -M main
git push -u origin main
```

If this remote already has the Cloud bootstrap commit, pull and merge first:

```bash
git pull origin main --rebase
git push origin main
```

## Cursor Cloud Agent

1. Open [Cloud Agents](https://cursor.com/dashboard/cloud-agents) → **Environments**.
2. Link or create an environment for **`samuelfreemanjobs-hash/instruments-Memphis-Drum-Loop-Factory`** (not Instruments).
3. Save the environment after the first successful build (`.cursor/environment.json` in this repo).
4. Start a new Cloud Agent run from this repository on `main`.

Install runs `scripts/cloud-agent-install.sh` (`pip install -r requirements.txt`, then `pytest --co-only` when `tests/` exists).

## Tests & lint

```bash
pytest tests/ -x -v
ruff check .
```
