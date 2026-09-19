# Expansion format

Sell **expansions** without rebuilding the plugin.

## Folder layout

```text
DL_EXPANSION_001/
├── manifest.json
├── artwork/
├── samples/          # optional raw (not loaded at runtime if baked to ROM)
├── multisamples/     # manifests / JSON
├── rom/              # pcm_bank_*.dlrrom for this package
├── patches/
├── performances/
├── fx/
└── metadata/
```

## manifest.json (minimum)

```json
{
  "contentId": "DL_EXP_001",
  "name": "Memphis After Dark",
  "version": "1.0.0",
  "engineVersion": "1.0.0",
  "contentVersion": 1,
  "schemaVersion": 1,
  "manifestVersion": 1
}
```

## Entitlement (future)

License system → entitlement list → **ContentEngine** mounts allowed `contentId` paths. **Never** on audio thread.

Example stub: [FactoryContent/expansions/DL_EXP_001/manifest.json](../../DisklordzRompler/FactoryContent/expansions/DL_EXP_001/manifest.json)

## Product line

```text
DISKLORDZ ROMPLER (product)
  └── DL_PKG_FACTORY (core ROM)
  └── DL_EXP_001 Memphis After Dark
  └── DL_EXP_002 Cyber Shift
  └── …
```
