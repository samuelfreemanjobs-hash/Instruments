# DiskLordz platform — seven systems

The commercial product is **not one plugin**. It is a platform:

| # | System | Repo home | Role |
|---|--------|-----------|------|
| 1 | **ROM specification** | `DisklordzSynth/Docs/DISKLORDZ_ROM_SPECIFICATION.md` | DLRROM01 wave ROM, cards, layers |
| 2 | **Content database** | `DisklordzSynth/include/disklordz/content/` | SampleAsset → Multisample → Tone → Patch IDs |
| 3 | **ROM Factory** | `DisklordzSynth/` tools | WAV → analyzed assets → ROM / expansions |
| 4 | **Content engine** | `DisklordzRompler/Source/Content/` | Mount factory + expansions, entitlements (non-RT) |
| 5 | **ROMpler engine** | `DisklordzRompler/Source/Engine/` | Real-time voices, macros, streaming cache |
| 6 | **Sound design agent** | `DisklordzSynth/Docs/SOUND_DESIGN_AGENT.md` | AI generates patch JSON, not arbitrary DSP |
| 7 | **Distribution** | (future) installer, license, updates | Outside audio thread |

```text
                    DISKLORDZ USER
                           │
         ┌─────────────────┴─────────────────┐
         │                                   │
    ROM FACTORY                          ROMPLER
  (DisklordzSynth)                   (JUCE plugin)
         │                                   │
   Raw WAV / synth                      ContentEngine
         ↓                                   ↓
   Sample database                      Browser / presets
         ↓                                   ↓
   Multisamples ──DLRROM──►           Streaming cache
         ↓                                   ↓
   Tones / patches                         Performance
         ↓                                   ↓
   Expansions (DL_EXP_*)                 Macros / FX
         │                                   │
         └─────────────────┬─────────────────┘
                           ↓
              Factory ROM + expansion packages
```

**Separation rule:** RAW AUDIO ≠ MULTISAMPLE ≠ TONE ≠ PATCH ≠ PRESET ≠ PERFORMANCE.  
Many presets reference one multisample (e.g. Grand Piano → Dark Grand, VHS Grand, Phonk Grand).

Index: [DisklordzSynth/Docs/CONTENT_PLATFORM_INDEX.md](../DisklordzSynth/Docs/CONTENT_PLATFORM_INDEX.md)
