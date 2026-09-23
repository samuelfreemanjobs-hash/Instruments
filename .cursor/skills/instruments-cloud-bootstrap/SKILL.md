---
name: instruments-cloud-bootstrap
description: Bootstrap and verify the Instruments Cloud Agent VM (cmake, submodules, DawDreamer, Vst3OfflineRender). Use when an agent starts on this repo, environment fails, or deps are missing.
---

# Instruments Cloud VM bootstrap

## When to use

- First task on a Cloud Agent run for **samuelfreemanjobs-hash/Instruments**
- `ModuleNotFoundError: dawdreamer`, missing `Vst3OfflineRender`, or empty submodules
- After changing `.cursor/environment.json`

## Run

```bash
bash .cursor/scripts/cloud-install.sh
```

Or manually:

```bash
git submodule update --init --recursive gearmulator-lane/gearmulator mpc-agent
cmake --build build -j --target Vst3OfflineRender Wave909_VST3
pip install -r gearmulator-lane/requirements-dawdreamer.txt
```

## Verify

```bash
test -x build/Vst3OfflineRender && build/Vst3OfflineRender --help 2>&1 | head -1 || true
python3 -c "import dawdreamer; print('dawdreamer', dawdreamer.__version__)"
test -f gearmulator-lane/gearmulator/CMakeLists.txt && echo gearmulator ok
```

## Notes

- **No Docker** in Cloud Agent pods — use scripts on VM; Docker is for local/CI only.
- ROM/firmware never in git — user secret path only.
- Full routing: [docs/CLOUD_AGENT_VM.md](../../../docs/CLOUD_AGENT_VM.md)
