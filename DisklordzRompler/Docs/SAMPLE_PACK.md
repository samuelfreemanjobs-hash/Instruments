# DLRROM01 — factory raw wave ROM

## Origin

All factory audio is **synthesized at build time** by the four **[DisklordzSynth](../../DisklordzSynth/)** engines (`DisklordzSynth_BuildRawRom`). The ROM is a flat **wave table + PCM pool** (JD-style multisample sets), not pre-mapped tone regions. No third-party samples or Roland/JD dumps are used.

| Rompler tone | Multisample set | Engine |
|--------------|-----------------|--------|
| 0 | 1 | **additive** |
| 1 | 2 | **karplus** |
| 2 | 3 | **wave** (looped waves) |
| 3 | 4 | **subtractive** |

Each set has eight root zones (MIDI 36–84). The plugin picks the closest root wave at note-on. Sixteen additional standalone waves (set id 0) live in the ROM for tooling and future programs.

## File layout

```text
RomHeader     (magic DLRROM01, version, waveCount, pcmFloatCount)
WaveEntry[]   (wave table: offsets, roots, loop points, engine id, multisample set id)
float pcm[]   (interleaved mono pool)
```

See `DisklordzSynth/include/disklordz/RawRomFormat.h` and `Source/Assets/RawRomBank.h`.

## Manual regeneration

```bash
cmake --build build -j --target DisklordzSynth_BuildRawRom
./build/DisklordzSynth/DisklordzSynth_BuildRawRom /tmp/test.dlrrom
```

Normal plugin builds run this automatically before `juce_add_binary_data`.

## Legacy DLROMPR1

`DisklordzSynth_BuildFactoryPack` still builds the older region-pack format for separate commercial `.dlrom` products; the rompler no longer embeds it.

## Future

- P1: import user WAV folders into a DLRROM01 builder
- P1: optional link to Disklordz SaaS kit downloads (out of repo)
