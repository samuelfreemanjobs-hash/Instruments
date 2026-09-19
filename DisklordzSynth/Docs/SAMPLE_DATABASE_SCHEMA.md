# Sample database schema

Identity is **never the filename**. Use stable content IDs (see `content/ContentIds.h`).

## Pipeline entities

```text
RAW AUDIO (WAV on disk)
    ↓ import + analyze
SAMPLE ASSET (database row, DL_SAMPLE_*)
    ↓ zone grouping
MULTISAMPLE (DL_MULTI_*)
    ↓ ROM program / layer binding
TONE (DL_TONE_*)
    ↓ four layers + macros
PATCH / INSTRUMENT (DL_PATCH_*)
    ↓ macro snapshot + browser metadata
PRESET (factory program slot)
    ↓ optional live layer
PERFORMANCE (future: splits, MIDI maps)
```

## SampleAsset

| Field | Type | Notes |
|-------|------|-------|
| id | `DL_SAMPLE_*` | Primary key |
| filename | string | Relative path in content package |
| category | taxonomy enum | PIANO, 808, … |
| instrument | string | e.g. `grand_piano` |
| source | string | mic, synth engine, field |
| rootNote | MIDI | Detected or tagged |
| keyRangeLow / keyRangeHigh | MIDI | Optional zone hint |
| velocityMin / velocityMax | 0–127 | Layer |
| sampleRate | Hz | e.g. 48000 |
| bitDepth | int | 16 / 24 / 32 |
| channels | int | 1 or 2 |
| lengthFrames | uint64 | |
| loopStart / loopEnd | frames | |
| loopEnabled | bool | |
| loudnessLufs | float | Optional |
| peakDbfs | float | |
| tuningCents | float | |
| roundRobinIndex | uint8 | |
| velocityLayerIndex | uint8 | |
| license | string | e.g. `original`, `licensed_xyz` |
| creator | string | |
| contentPackage | `DL_PKG_*` | Factory or expansion |

## Multisample

| Field | Type |
|-------|------|
| id | `DL_MULTI_*` |
| name | string |
| category | taxonomy |
| zoneSampleIds | `DL_SAMPLE_*`[] |
| romBankIndex | uint8 | Target PCM card |
| multisampleSetId | uint16 | Program on card |
| metadata | tags, tier, layer |

## Patch

| Field | Type |
|-------|------|
| id | `DL_PATCH_*` |
| name | string |
| toneIds | `DL_TONE_*` × 4 (or embedded tone refs) |
| macros | 8 floats + mapping curves |
| fx | send/insert ids (future) |
| contentPackage | `DL_PKG_*` |
| taxonomy | genre, mood, era, … |

## Storage (production)

- **P0:** JSON/SQLite export from ROM Factory (not in git at scale)
- **Runtime:** ROMpler loads compiled `.dlrrom` + expansion manifests; full DB lives in factory tooling

C++ structs: `content/SampleAssetRecord.h`, `MultisampleRecord.h`, `PatchRecord.h`
