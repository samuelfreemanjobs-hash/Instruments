# DISKLORDZ ROMPLER — architecture

Sample-based four-tone rompler VSTi (not WAVE-909). Repo index: [ARCHITECTURE.md](../ARCHITECTURE.md).

## Purpose

Dark trap / phonk rompler: four layered tones (bell, keys, pad, sub) driven by a **clean-room generated** sample pack (`DLROMPR1`), with performance macros and factory presets.

## Build and run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target DisklordzRompler_VST3 DisklordzRompler_Standalone DisklordzRomplerTests
ctest --test-dir build -R DisklordzRompler
```

Artifact: `build/DisklordzRompler/DisklordzRompler_artefacts/Release/VST3/DISKLORDZ ROMPLER.vst3`

## Sample pack pipeline

```text
GenerateDisklordzRomplerPack (offline synth)
        ↓
disklordz_factory.dlrom (DLROMPR1)
        ↓
juce_add_binary_data → embedded in plugin
        ↓
SampleBank at startup (no audio-thread load)
```

Regenerate only via rebuild; see [Docs/SAMPLE_PACK.md](Docs/SAMPLE_PACK.md).

## Data flow

```text
MIDI → DisklordzRomplerProcessor → RomplerEngine → RomplerVoice × 24
     → 4 tone layers × SampleBank region → mix → macro DSP → out
```

## Threading

| Thread | Work |
|--------|------|
| Audio | Voice render, parameter reads from APVTS atomics |
| Message | Editor, preset changes |
| Build-time | `GenerateDisklordzRomplerPack` procedural PCM |

## Key modules

| Path | Responsibility |
|------|----------------|
| `tools/GenerateRomplerPack.cpp` | Procedural bell/keys/pad/sub multisamples |
| `Source/Assets/PackFormat.h`, `SampleBank.h` | DLROMPR1 parse + region lookup |
| `Source/Engine/RomplerEngine.h` | Voice pool, MIDI sustain/bend |
| `Source/Engine/RomplerVoice.h` | Per-layer sample playback |
| `Source/Presets/FactoryPresets.*` | Factory programs |
| `Source/PluginProcessor.*` | APVTS, pack embed, host glue |

## Extension points

- User packs: message-thread file load into `SampleBank` (P1)
- UI tabs from mock: [Docs/UI_SPEC.md](Docs/UI_SPEC.md)
- SaaS kit download hooks (document only)

## Related docs

- [Docs/PRODUCT_SPEC.md](Docs/PRODUCT_SPEC.md)
- [Docs/SAMPLE_PACK.md](Docs/SAMPLE_PACK.md)
