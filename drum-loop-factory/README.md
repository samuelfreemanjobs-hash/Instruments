# Drum Loop Factory OS

DDSP agent and ML pipeline for 1990s Memphis phonk drum loops.

Use this tree as the **root of its own GitHub repository** for Cursor Cloud Agent (not the Instruments JUCE monorepo).

## Connect local repo → GitHub

Your machine may have **no remotes** yet (`git remote -v` empty). Pick **one** remote:

| Option | GitHub URL |
|--------|------------|
| **Existing empty repo** (recommended) | `https://github.com/samuelfreemanjobs-hash/instruments-Memphis-Drum-Loop-Factory.git` |
| **New repo name** `drum-loop-factory` | `https://github.com/samuelfreemanjobs-hash/drum-loop-factory.git` |

Create the GitHub repo **without** a generated README or license if you already have a full local history.

### Option A — HTTPS (replace URL if you chose the new repo name)

```bash
cd /path/to/drum-loop-factory   # e.g. Desktop/Projects/drum-loop-factory
git remote add origin https://github.com/samuelfreemanjobs-hash/instruments-Memphis-Drum-Loop-Factory.git
git branch -M main
git push -u origin main
```

### Option B — GitHub CLI

```bash
cd /path/to/drum-loop-factory
gh repo create drum-loop-factory --private --source=. --remote=origin --push
# Or push to the existing Memphis repo:
# gh repo set-default samuelfreemanjobs-hash/instruments-Memphis-Drum-Loop-Factory
# git remote add origin https://github.com/samuelfreemanjobs-hash/instruments-Memphis-Drum-Loop-Factory.git
# git push -u origin main
```

If the remote already has commits (e.g. Cloud bootstrap), pull before push:

```bash
git pull origin main --rebase
git push origin main
```

Cloud bootstrap files (`.cursor/environment.json`, `scripts/cloud-agent-install.sh`) also live on Instruments PR branch `cursor/drum-loop-factory-cloud-pointer-4d0e` under `drum-loop-factory/` — copy or merge them into your local tree if they are not already present.

## Connect Cursor Cloud Agent

Cloud Agent runs on a remote VM and checks out **one GitHub repo** into `/workspace`.

1. Push your local project to GitHub (steps above).
2. Open [Cloud Agents → Environments](https://cursor.com/dashboard/cloud-agents/environments).
3. Create or edit an environment whose **repository** is your factory repo (e.g. `samuelfreemanjobs-hash/instruments-Memphis-Drum-Loop-Factory`), **not** `Instruments`.
4. Ensure the repo contains `.cursor/environment.json` (this folder includes a Python 3.11 template). Trigger a **build** on `main` and **Save** when install succeeds.
5. Start a **new Cloud Agent** run from that repository.

After checkout, the agent loads project rules from the repo, including:

- `.cursorrules` (if present)
- `.cursor/rules/drum-loop-factory.mdc` (if present)
- `AGENTS.md`, `contracts/loop_types.py`, and `docs/ccp/` for DPCS/CCP gates

Install (from `.cursor/environment.json`): `bash scripts/cloud-agent-install.sh` → `pip install -r requirements.txt`.

## Tests & lint

```bash
pytest tests/ -x -v
ruff check .
```
