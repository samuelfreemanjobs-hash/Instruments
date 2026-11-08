# VST preset factory (Preset Agent SOP)

Each **JUCE instrument** in this monorepo targets **≥ 256** factory programs. Expansion is **deterministic procedural variation** from curated “seed” presets (trap / plugg / phonk lanes), not hand-written one-offs.

## Products

| Product | Bank | Implementation |
|---------|------|------------------|
| **JD Upgraded** | 256 patches | `Source/Assets/FactoryPatchLibrary.cpp` (`kFactoryPatchCount`) |
| **WAVE-9090** | 256 programs | `Wave9090/Source/Presets/FactoryPresetBank.cpp` |
| **Plugg VA** | 256 programs | `PluggVA/Source/Presets/FactoryPresetBank.cpp` |
| **SP-1200** | 256 kit templates | `SP1200/Source/Presets/FactoryProgramBank.cpp` |

## Agent workflow (Preset Agent)

1. Edit **curated seeds** only (`buildCuratedSeeds()` in each `FactoryPresetBank.cpp` or JD category builders).
2. Procedural fill to `kFactoryPresetCount` (256) must stay **deterministic** (same index → same params).
3. Verify counts:

```bash
python3 scripts/vst/verify_factory_preset_counts.py
ctest --test-dir build -R "PluggVA|Wave9090|SP1200" --output-on-failure
```

Hardware SysEx batches remain **`hardware-preset-designer`** (`tools/synth-forge/`).

## Related

- [PluggVA/Docs/PRODUCT_SPEC.md](../PluggVA/Docs/PRODUCT_SPEC.md)
- [Wave9090/Docs/PRODUCT_SPEC.md](../Wave9090/Docs/PRODUCT_SPEC.md)
