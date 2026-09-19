# DisklordzSynth

Offline **procedural synth engines** for building sellable sample content and `.dlrom` packs. Independent from the DISKLORDZ ROMPLER plugin — keep, extend, and productize these tools on their own.

## Quick start

```bash
# One-shot bell WAV (48000 Hz)
./build/DisklordzSynth/DisklordzSynth_RenderWav bell 4096 /tmp/bell.wav 0.5 4.0

# Full factory multisample folder (32 WAVs)
./build/DisklordzSynth/DisklordzSynth_ExportWavPack /tmp/disklordz-factory-wavs

# Binary pack for rompler / custom loaders
./build/DisklordzSynth/DisklordzSynth_BuildFactoryPack /tmp/factory.dlrom
```

## Docs

- [ARCHITECTURE.md](ARCHITECTURE.md)
- [Docs/COMMERCIAL.md](Docs/COMMERCIAL.md)
