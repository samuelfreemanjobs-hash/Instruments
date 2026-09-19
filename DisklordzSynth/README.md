# DisklordzSynth

Offline **procedural synth engines** for building sellable sample content and `.dlrom` packs. Independent from the DISKLORDZ ROMPLER plugin — keep, extend, and productize these tools on their own.

## Quick start

```bash
# Four engines: subtractive | additive | karplus | wave
./build/DisklordzSynth/DisklordzSynth_RenderWav additive 4096 /tmp/bell.wav 0.5 0.4
./build/DisklordzSynth/DisklordzSynth_RenderWav karplus 8192 /tmp/pluck.wav 0.99 0.3
./build/DisklordzSynth/DisklordzSynth_RenderWav wave 16384 /tmp/pad.wav 0.4 0.25
./build/DisklordzSynth/DisklordzSynth_RenderWav subtractive 12000 /tmp/sub.wav 0.2 0.25

# Full factory multisample folder (32 WAVs)
./build/DisklordzSynth/DisklordzSynth_ExportWavPack /tmp/disklordz-factory-wavs

# Binary pack for rompler / custom loaders
./build/DisklordzSynth/DisklordzSynth_BuildFactoryPack /tmp/factory.dlrom
```

## Docs

- [ARCHITECTURE.md](ARCHITECTURE.md)
- [Docs/COMMERCIAL.md](Docs/COMMERCIAL.md)
