# HISE sketch projects (Disklordz)

## Purpose

Hold **optional** HISE instrument projects (romplers, sample-based plugins) built by the **Antigravity** sketch lane. This tree is empty until a sketch is approved for version control.

## Build & run

Not built by root CMake. On **Windows**:

1. Open project in HISE or use Antigravity tools.
2. `export_ci` + `Binaries/batchCompile.bat` → `.vst3` under `Binaries/Compiled/`.

Full procedure: [docs/HISE_SKETCH_LANE.md](../docs/HISE_SKETCH_LANE.md).

## Data flow

Brief (Airtable / Marketing) → HiseScript + samples → XML preset → HISE export → VST3 binary → (optional) git import under `hise-sketch/<ProductName>/`.

## Threading / realtime

Standard HISE/JUCE plugin rules apply in exported binaries; agent work is **offline** on the scripting/export path.

## Key modules

| Path | Responsibility |
|------|----------------|
| `docs/HISE_SKETCH_LANE.md` | Lane policy, gates, CLI, roles |
| `<ProductName>/Scripts/` | HiseScript (agent-written) |
| `<ProductName>/XmlPresetBackups/` | Saved project XML |

## Extension points

- New product: Planner + Marketing brief → Antigravity sketch → promote row in Airtable → import folder here or separate repo.
- JUCE ship: open **port WO** in main factory; do not duplicate Junova/NovaDrum IDs.

## Related docs

- [docs/DISKLORDZ_PLUGIN_TRACKS.md](../docs/DISKLORDZ_PLUGIN_TRACKS.md)
- [disklordz/airtable/seed/hise-sketch-lane-2026.json](../disklordz/airtable/seed/hise-sketch-lane-2026.json)
