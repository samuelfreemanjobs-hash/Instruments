# Grim-MIDI — architecture

## Layout

| Path | Role |
|------|------|
| `vst/GrimMIDI/Source/GrimChordEngine.*` | Scale + voicing (no heap) |
| `Source/ProgressionPresets*.h` | 100 constexpr progression bundles |
| `Source/ProgressionMidiBuilder.*` | Shared offline + GUI MIDI export |
| `Source/PluginProcessor.*` | MIDI FX, APVTS, voice tracking |
| `tools/ExportMidiPack.cpp` | Batch `.mid` writer |
| `tools/grimmidi/generate_progression_presets.py` | Regenerate preset table |

## Realtime

- `prepareToPlay`: `MidiBuffer::ensureSize`
- `processBlock`: strum via `addEvent` offsets only; no allocations

## Build

```bash
cd vst/GrimMIDI && cmake -B build && cmake --build build -j
```

MIDI pack:

```bash
bash tools/grimmidi/export_midi_pack.sh
```

## Verification

- `GrimMIDIProcessorTests` (ctest)
- `.github/workflows/grimmidi.yml` (Linux + Windows pluginval 5)
