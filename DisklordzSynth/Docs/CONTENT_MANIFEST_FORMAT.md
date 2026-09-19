# Content manifest format

Every installable package (factory core or expansion) includes a **manifest** for versioning and migration.

## Required fields

| Field | Example | Purpose |
|-------|---------|---------|
| contentId | `DL_PKG_FACTORY` / `DL_EXP_001` | Stable package id |
| name | `DiskLordz Core` | Display |
| version | `1.0.0` | Semver |
| engineVersion | `1.0.0` | Minimum ROMpler |
| contentVersion | `1` | Integer bump for migrations |
| schemaVersion | `1` | JSON/schema for patches |
| manifestVersion | `1` | This file format |

## Example (factory core)

```json
{
  "contentId": "DL_PKG_FACTORY",
  "name": "DiskLordz Core Factory ROM",
  "version": "1.0.0",
  "engineVersion": "1.0.0",
  "contentVersion": 1,
  "schemaVersion": 1,
  "manifestVersion": 1,
  "romCards": ["pcm_bank_00.dlrrom", "pcm_bank_01.dlrrom"],
  "presetCatalog": "presets/catalog.json"
}
```

## Migration rule

ROMpler **1.1** must load **expansion 1.0** if `engineVersion` ≤ host and `schemaVersion` is supported. Breaking schema → migration tool in ROM Factory, not silent failure.

Parser stub: `content/ExpansionManifest.h`
