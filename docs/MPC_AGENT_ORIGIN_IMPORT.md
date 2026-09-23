# MPC-agent + DawDreamer — Origin handoff import

Your full **MPC-agent** and **DawDreamer** tree lives in a Cursor Origin tmp export:

`https://origin.cursor.com/git/samuelfreemanjobs/tmp-1e342cd8506744c8.git`

Cloud Agents need **`origin.cursor.com`** on the environment egress allowlist. This is declared in [`.cursor/environment.json`](../.cursor/environment.json) (`egressAllowlist` + `repositoryDependencies` for the tmp handoff). After you **Save** the environment in the [Cloud dashboard](https://cursor.com/dashboard/cloud-agents/environments/e/85d21659-b124-11f1-a3d8-362438fd9788), start a **new** agent run to clone Origin. Until then, mirror the repo to GitHub (below).

## Recommended: mirror to GitHub (one time)

On a machine that can open Origin (Cursor desktop / local git):

```bash
git clone https://origin.cursor.com/git/samuelfreemanjobs/tmp-1e342cd8506744c8.git mpc-handoff
cd mpc-handoff
git remote add instruments https://github.com/samuelfreemanjobs-hash/Instruments.git
git fetch instruments cursor/instrument-factory-agent-plan-d805
git checkout -b cursor/mpc-dawdreamer-handoff-d805
# Optional: move tree under disklordz/mpc-agent/ before push
mkdir -p disklordz/mpc-agent && rsync -a --exclude .git ./ disklordz/mpc-agent/
git add disklordz/mpc-agent && git commit -m "Import MPC-agent + DawDreamer from Origin handoff"
git push -u instruments cursor/mpc-dawdreamer-handoff-d805
```

Then open a Cloud Agent on that branch (or merge into `cursor/instrument-factory-agent-plan-d805`).

## In-repo merge target

| Path | Purpose |
|------|---------|
| `disklordz/mpc-agent/` | MPC-agent rules, skills, scripts (after import) |
| `disklordz/sound-factory/scripts/DAWDreamer_renderer.py` | Factory entry (replace/merge with Origin copy) |
| `disklordz/sound-factory/scripts/renderers/` | DawDreamer implementation |

After import, run:

```bash
bash disklordz/sound-factory/scripts/merge_dawdreamer_handoff.sh --check
python3 disklordz/sound-factory/tests/run_tests.sh
```

Update [AGENTS.md](../AGENTS.md) with the final `mpcAgent.repoPath`: `disklordz/mpc-agent`.

## Stub vs handoff

The factory branch already contains a **minimal** DawDreamer host (`DAWDreamer_renderer.py`). The Origin handoff should **supersede** those files where your MPC-agent version is richer (preset dispatch, batch drivers, MPCTK glue).
