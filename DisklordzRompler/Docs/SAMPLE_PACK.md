# DLROMPR1 — factory sample pack

## Origin

All factory audio is **synthesized at build time** by `tools/GenerateRomplerPack.cpp`. No third-party samples or Roland/JD dumps are used.

Categories per tone slot:

| Tone | Category | Generator |
|------|----------|-----------|
| 0 | Bell | Inharmonic partial stacks + decay |
| 1 | Keys | Karplus–Strong plucks |
| 2 | Pad | Filtered noise loops |
| 3 | Sub | Sine 808-style decays |

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
cmake --build build -j --target GenerateDisklordzRomplerPack
./build/DisklordzRompler/GenerateDisklordzRomplerPack /tmp/test.dlrom
```

Normal builds run this automatically before `juce_add_binary_data`.

## Future

- P1: import user WAV folders with manifest JSON
- P1: optional link to Disklordz SaaS kit downloads (out of repo)
