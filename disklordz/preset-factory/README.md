# Preset Factory

Generates plugin preset packs from **Sound DNA** + plugin parameter maps.

## Outputs

- Factory / demo / expansion / random presets  
- Feeds **Product Compiler** target `preset_pack`  
- Versioned with plugin semver (`preset_pack_version` in manifest)

## Inputs

- `plugin-kernel/products/<slug>/` parameter IDs  
- Kit assets tagged in DNA for genre/character alignment  

## Pipeline (planned)

```text
kit DNA + plugin manifest
    → preset recipes (JSON)
    → render preset files
    → QA: load all presets in pluginval/host
```

See [PLUGIN_KERNEL.md](../docs/PLUGIN_KERNEL.md) and [PRODUCT_COMPILER.md](../docs/PRODUCT_COMPILER.md).
