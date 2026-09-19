# DISKLORDZ ROMPLER — architecture

Sample-based four-tone rompler VSTi (not WAVE-909). Repo index: [ARCHITECTURE.md](../ARCHITECTURE.md).

## Purpose

Dark trap / phonk rompler: four layered tones (additive, karplus, wave, subtractive ROM banks) driven by a **clean-room generated** raw wave ROM (`DLRROM01`), with performance macros and factory presets.

## Build and run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target DisklordzRompler_VST3 DisklordzRompler_Standalone DisklordzRomplerTests
ctest --test-dir build -R DisklordzRompler
```

Artifact: `build/DisklordzRompler/DisklordzRompler_artefacts/Release/VST3/DISKLORDZ ROMPLER.vst3`

## Raw ROM pipeline

```text
DisklordzSynth (4 engines) → 16× buildRawRomBank()
        ↓
pcm_bank_00…15.dlrrom (DLRROM01 v2, ~448 waves/card)
        ↓
POST_BUILD → VST3/Standalone Resources/DisklordzRom/
        ↓
RawRomLibrary load at startup (no audio-thread load)
```

Regenerate only via rebuild; see [Docs/SAMPLE_PACK.md](Docs/SAMPLE_PACK.md).

## Data flow

```text
MIDI → DisklordzRomplerProcessor → RomplerEngine → RomplerVoice × 24
     → 4 tone layers × ROM wave select (multisample set) → mix → macro DSP → out
```

## Threading

| Thread | Work |
|--------|------|
| Audio | Voice render, parameter reads from APVTS atomics |
| Message | Editor, preset changes |
| Build-time | `DisklordzSynth_BuildRawRom` procedural PCM |

## Key modules

| Path | Responsibility |
|------|----------------|
| `DisklordzSynth/src/RawRomBuilder.cpp` | Engine-only factory ROM synthesis |
| `Source/Assets/RawRomBank.h` | DLRROM01 parse + multisample wave lookup |
| `Source/Engine/RomplerEngine.h` | Voice pool, MIDI sustain/bend |
| `Source/Engine/RomplerVoice.h` | Per-layer wave playback |
| `Source/Presets/FactoryPresets.*` | Factory programs |
| `Source/PluginProcessor.*` | APVTS, ROM embed, host glue |

## Extension points

- User ROM: message-thread load into `RawRomBank` (P1)
- UI tabs from mock: [Docs/UI_SPEC.md](Docs/UI_SPEC.md)
- SaaS kit download hooks (document only)

## Seven-system platform

ROM Factory (DisklordzSynth) and ROMpler (this plugin) are **separate systems**. See [docs/DISKLORDZ_PLATFORM.md](../docs/DISKLORDZ_PLATFORM.md).

Entity chain: **SampleAsset → Multisample → Tone → Patch → Preset → Performance** (IDs, not filenames).

## Content engine (P0+)

- [Source/Content/ContentEngine.h](Source/Content/ContentEngine.h) — mount `DL_PKG_FACTORY` + `DL_EXP_*` (message thread)
- [Source/Content/StreamingSampleCache.h](Source/Content/StreamingSampleCache.h) — multi-GB streaming policy (stub)
- [Docs/STREAMING_ARCHITECTURE.md](Docs/STREAMING_ARCHITECTURE.md)

License / entitlement must **never** run on the audio thread.

## ROM Factory (commercial scale)

Raw WAV → multisample manifests → `.dlrrom` cards → tones → 4-tone patches → presets. See [Docs/ROM_FACTORY.md](Docs/ROM_FACTORY.md) and [FactoryContent/README.md](FactoryContent/README.md).

## Related docs

- [Docs/PRODUCT_SPEC.md](Docs/PRODUCT_SPEC.md)
- [Docs/SAMPLE_PACK.md](Docs/SAMPLE_PACK.md)
- [Docs/ROM_FACTORY.md](Docs/ROM_FACTORY.md)
