# Factory content (not shipped in git at scale)

Production WAV libraries live outside git (or in private LFS). This folder documents **layout + manifest format** for the ROM Factory.

## Example layout

```text
Piano_001/
├── multisample.manifest
└── samples/
    ├── PP/C1.wav
    ├── PP/G1.wav
    └── MF/C4.wav
```

## `multisample.manifest` (line format)

```text
# Source multisample — maps to DLRROM wave table entries (not one giant WAV)
id piano_concert_grand_001
family acoustic_piano
layer real
tier hero
program 1
zone 36 samples/PP/C1.wav 0 127
zone 48 samples/PP/G1.wav 0 127
zone_looped 60 samples/MF/C4.wav
```

- **family** — snake_case from ROM_FACTORY.md  
- **layer** — `real`, `classic_digital`, `analog`, `lo_fi`, `cyber_shift`  
- **program** — multisample set id on the ROM card (1…128)  
- **zone** — root MIDI note, relative wav path, optional velocity min/max  

Build:

```bash
DisklordzSynth_RomFactory build-manifest Piano_001/ /tmp/pcm_bank_custom.dlrrom 0
```

## Preset fan-out

One folder like `Piano_001/` is **one source multisample**. Patch/preset JSON (future) references the same `id` with different macro blocks — see `PatchPipeline.h`.
