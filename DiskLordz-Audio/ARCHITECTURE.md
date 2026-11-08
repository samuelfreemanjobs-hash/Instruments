# DiskLordz Audio (DPF product tree)

## Purpose

Shared **DiskLordz Plugin Framework (DPF)** and **Products/** SKUs for trap/phonk/underground MIDI and FX plugins. CHORD ENGINE is the first shipping product.

## Build & run

Milestone 001 (in progress): Factory adds root `CMakeLists.txt` + `Products/ChordEngine` JUCE target.

```bash
# After Factory lands CMake (Windows x64)
cmake -B DiskLordz-Audio/Build/Windows -S DiskLordz-Audio
cmake --build DiskLordz-Audio/Build/Windows --config Release
```

Until then: structure-only; see [MILESTONE_001_FACTORY.md](MILESTONE_001_FACTORY.md).

## Data flow

```text
Products/ChordEngine  →  DPF-MIDI (theory, chords, progressions)  →  DPF-Core (params, presets)
        →  JUCE adapter  →  VST3  →  DAW MIDI out
```

## Threading / realtime

DSP/MIDI engines must not allocate on the audio callback; presets/UI on message thread per DPF rules in `docs/DPF_TECHNICAL_ARCHITECTURE.md`.

## Key modules

| Path | Role |
|------|------|
| `DPF-Core/` | Parameters, presets, state, licensing hooks |
| `DPF-MIDI/` | Theory + chord + progression engines |
| `DPF-DSP/` | Future FX SKUs |
| `Products/ChordEngine/` | Flagship MIDI plugin |
| `Tests/` | Theory/MIDI/plugin tests |
| `Build/` | Out-of-tree build dirs |
| `LICENSES/` | Third-party notices |

## Extension points

Add SKUs under `Products/<Name>/`; version independently. Keep product logic out of JUCE glue where possible.

## Related docs

- [docs/DISKLORDZ_MASTER_PLAN.md](../docs/DISKLORDZ_MASTER_PLAN.md)
- [docs/CHORD_ENGINE_PRD.md](../docs/CHORD_ENGINE_PRD.md)
