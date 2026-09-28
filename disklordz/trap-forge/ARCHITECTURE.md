# TRAP-FORGE (browser + JUCE VST3)

Procedural trap drum one-shots shared between the **browser studio** (when merged from `cursor/mpc-trap-forge-pwa-453b`) and this **native shell**.

## Purpose

Factory JSON presets (`schema/trapforge-preset.schema.json`) drive layered kick, 808 glide, and snare noise renders in the VST3 instrument.

## Build & run

From monorepo root (after JUCE fetch):

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target TrapForge_VST3 TrapForge_Standalone
./scripts/sync-compile-commands.sh   # clangd at repo root
```

## Data flow

```text
Host MIDI note-on
  → TrapForgeAudioProcessor (factory program → TrapForgePreset)
  → renderDrumMono() (DrumRender.cpp)
  → stereo mix + output ceiling
```

Preset JSON on disk: `presets/factory/*.json` (embedded defaults also in `TrapForgePreset.cpp`).

## Key modules

| Path | Role |
|------|------|
| `schema/trapforge-preset.schema.json` | Shared preset contract (JS + C++) |
| `plugin/Source/TrapForgePreset.*` | JSON → struct, factory list |
| `plugin/Source/DrumRender.*` | Offline mono render per category |
| `plugin/Source/PluginProcessor.*` | MIDI trigger, program change |
| `presets/factory/` | Example factory JSON |

## Extension points

- Load `presets/factory/*.json` at runtime instead of embedded strings.
- Port elite JS snare/clap layers (+2–4 st clap stack) per [docs/SNARE_RESEARCH_PLAN.md](../../docs/SNARE_RESEARCH_PLAN.md).
- Golden compare via `tools/Vst3OfflineRender` once CI bundles TrapForge.

## Related docs

- [docs/SNARE_RESEARCH_PLAN.md](../../docs/SNARE_RESEARCH_PLAN.md)
- [disklordz/ARCHITECTURE.md](../ARCHITECTURE.md)
