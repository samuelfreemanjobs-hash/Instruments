# Patch format

A **patch** (instrument) is four **tones** + macro defaults + metadata. Many **presets** are macro/browser variants of the same patch.

## ID

`DL_PATCH_000001`

## Structure

```json
{
  "schemaVersion": 1,
  "id": "DL_PATCH_000042",
  "name": "Midnight Digital Bell",
  "contentPackage": "DL_PKG_FACTORY",
  "tones": [
    { "toneId": "DL_TONE_000110", "level": 1.0 },
    { "toneId": "DL_TONE_000111", "level": 0.6 },
    { "toneId": "DL_TONE_000220", "level": 0.35 },
    { "toneId": "DL_TONE_000330", "level": 0.5 }
  ],
  "macros": {
    "character": 0.55,
    "brightness": 0.4,
    "drive": 0.35,
    "space": 0.5,
    "movement": 0.3,
    "width": 0.5,
    "decay": 0.4,
    "crush": 0.25
  },
  "taxonomy": {
    "instrumentType": "BELL",
    "genre": ["wave_phonk"],
    "mood": ["dark"],
    "era": "90s_digital"
  }
}
```

## Tone reference

```json
{
  "id": "DL_TONE_000110",
  "multisampleId": "DL_MULTI_000015",
  "romBank": 12,
  "program": 3
}
```

Runtime maps to `RomplerParams.toneRomBank` + `toneProgram` + levels.

## Preset vs patch

| Preset | Patch | Notes |
|--------|-------|-------|
| “Dark Cinema Piano” | Same as “Grand Piano” patch | Different macros/names |
| Factory program slot | `PresetCatalogEntry` | 1,000+ entries OK |

C++: `content/PatchRecord.h`, `PatchPipeline.h` (legacy descriptors migrating to IDs).
