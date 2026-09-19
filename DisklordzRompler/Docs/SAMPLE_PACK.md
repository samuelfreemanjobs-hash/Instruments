# DLROMPR1 — factory sample pack

## Origin

All factory audio is **synthesized at build time** by **[DisklordzSynth](../../DisklordzSynth/)** (`DisklordzSynth_BuildFactoryPack`). No third-party samples or Roland/JD dumps are used. The synth tools are retained for separate WAV/pack products — see [DisklordzSynth/Docs/COMMERCIAL.md](../../DisklordzSynth/Docs/COMMERCIAL.md).

Categories per tone slot:

| Tone | Category | DisklordzSynth engine |
|------|----------|------------------------|
| 0 | Bell | **additive** |
| 1 | Keys | **karplus** |
| 2 | Pad | **wave** (looped) |
| 3 | Sub | **subtractive** |

Eight velocity/key zones per tone (roots 36–84) → 32 regions total.

## File layout

```text
PackHeader   (magic DLROMPR1, version, counts)
SampleRegion[regionCount]
float pcm[pcmFloatCount]
```

See `Source/Assets/PackFormat.h`.

## Manual regeneration

```bash
cmake --build build -j --target DisklordzSynth_BuildFactoryPack
./build/DisklordzSynth/DisklordzSynth_BuildFactoryPack /tmp/test.dlrom
```

Normal builds run this automatically before `juce_add_binary_data`.

## Future

- P1: import user WAV folders with manifest JSON
- P1: optional link to Disklordz SaaS kit downloads (out of repo)
