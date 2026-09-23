---
name: gearmulator-multisample-capture
description: Capture multisamples from Gearmulator or factory VST3 on the Cloud VM (DawDreamer, Vst3OfflineRender, capture plans, Docker docs for local). Use for Track E, JE-8086, Osirus, ROM-adjacent render jobs.
---

# Skill: Gearmulator multisample capture

## Read first

- [docs/GEARMULATOR_LANE.md](../../../docs/GEARMULATOR_LANE.md)
- [docs/GEARMULATOR_MULTISAMPLING.md](../../../docs/GEARMULATOR_MULTISAMPLING.md)
- [docs/GEARMULATOR_FORK.md](../../../docs/GEARMULATOR_FORK.md) — submodule: **samuelfreemanjobs-hash/gearmulator**

## Bootstrap

Run skill **instruments-cloud-bootstrap** if submodules or DawDreamer missing.

## Capture (Python — preferred on VM)

Each session outputs **`instrument.map.json`** (canonical) + `manifest.tsv` — see [docs/MULTISAMPLE_MAP_FORMAT.md](../../../docs/MULTISAMPLE_MAP_FORMAT.md).

```bash
python3 scripts/gearmulator/render_multisample_dawdreamer.py \
  --plugin /path/to/Plugin.vst3 \
  --instrument-label YourNewPreset_Name \
  --name session1 --program 0 \
  --note-start 48 --note-end 72 --note-step 1 --velocity 100
```

Batch plan: edit `gearmulator-lane/multisamples/capture-plan.example.tsv` →

```bash
python3 scripts/gearmulator/render_capture_plan.py --plan my-plan.tsv
```

## Capture (C++ fallback)

```bash
./scripts/gearmulator/render_multisample_grid.sh --plugin "/path/to/Plugin.vst3" --name session1
```

## Build Gearmulator VST3 (fork)

```bash
git submodule update --init --recursive gearmulator-lane/gearmulator
./scripts/gearmulator/build-je8086-console.sh   # smoke without ROM
# Full VST3: upstream build_linux.sh inside submodule after ROM on disk
```

## Subagents

| Situation | Subagent |
|-----------|----------|
| Upstream CMake / device code | `explore` in `gearmulator-lane/gearmulator` |
| Render fails only for one plugin | `debug` |
| Preset needs GUI tweak once | `computerUse` (sparingly) |

## Never

- Commit ROM, `.mid` firmware dumps, or `multisamples/out/*.wav`
- Copy GPLv3 `jeLib` into `Source/`
