# Cleanroom ROM (`jdupg_cleanroom.rom`)

JD Upgraded ships an **original** PCM wave ROM — not a Roland JD-800/990 dump. All waves are synthesized at build time by `tools/GenerateCleanroomRom.cpp` (additive spectra, Karplus–Strong plucks, formant stacks, noise loops, etc.).

## Format (`JDUPGROM` v1)

| Field | Description |
|--------|-------------|
| Header | Magic, version, wave count, table/PCM offsets, CRC32 over PCM |
| Wave table | 128 entries: frame count, loop points, flags, PCM offset, root note, category |
| PCM | Little-endian `int16` mono samples |

Categories rotate across waves: analog, digital, bell, pluck, bass, noise, vocal, organ, fx.

## Build integration

CMake runs `GenerateCleanroomRom` and embeds the file via `juce_add_binary_data`. The plugin loads it once at startup (`RomBank` → float pool, no audio-thread allocation).

## Regenerate manually

```bash
cmake -B build -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build --target GenerateCleanroomRom
./build/GenerateCleanroomRom Resources/Cleanroom/jdupg_cleanroom.rom
```

## Legal

Do not redistribute proprietary hardware ROM images. Optional future loaders may accept **user-owned** dumps for personal use; the factory asset remains this cleanroom library only.
