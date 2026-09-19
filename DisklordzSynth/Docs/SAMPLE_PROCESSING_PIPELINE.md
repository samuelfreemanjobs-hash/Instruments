# Sample processing pipeline

ROM Factory stages (automate over time):

```text
IMPORT WAV
    ↓
ANALYZE (peak, loudness, DC, sample rate)
    ↓
PITCH / ROOT DETECT
    ↓
TRIM
    ↓
NORMALIZE / GAIN (per standard)
    ↓
LOOP DETECT → OPTIMIZE → CROSSFADE
    ↓
CREATE SampleAsset (DL_SAMPLE_*)
    ↓
GROUP → Multisample (DL_MULTI_*)
    ↓
BUILD TONE (DL_TONE_*)
    ↓
BUILD PATCH (DL_PATCH_*)
    ↓
GENERATE MACROS / METADATA (agent or templates)
    ↓
VALIDATE (QC_VALIDATION.md)
    ↓
WRITE DLRROM + manifest
    ↓
INSTALL (content package)
```

## CLI (current)

| Command | Stage |
|---------|--------|
| `DisklordzSynth_RomFactory build-manifest` | WAV folder → `.dlrrom` |
| `DisklordzSynth_RomFactory build-procedural` | Placeholder PCM cards |
| `DisklordzSynth_RomFactory validate-package` | Manifest + ID checks (stub) |

## Future tools

- `import-wav` → SQLite/JSON DB
- `analyze` → fill loop/root fields
- `build-multisample` → from DB
- `build-expansion` → `DL_EXP_*` folder

Implementation grows in `DisklordzSynth/` independently of JUCE plugin.
