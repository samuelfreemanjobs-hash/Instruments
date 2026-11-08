# VST preset factory (Preset Agent SOP)

Each **JUCE instrument** in this monorepo targets **≥ 512** factory programs. Expansion is **deterministic procedural variation** from curated “seed” presets (trap / plugg / phonk lanes), not hand-written one-offs.

**Fleet agent:** `vst-preset-agent` — `disklordz/agents/profit/vst-preset-agent/agent.md`, skill `disklordz-vst-preset-agent`, CLI `python3 disklordz/integrations/agents/vst_preset_agent.py`.

## Products

| Product | Bank | Implementation |
|---------|------|------------------|
| **JD Upgraded** | 512 patches | `Source/Assets/FactoryPatchLibrary.cpp` (`kFactoryPatchCount`) |
| **WAVE-9090** | 512 programs + **114 wavetables** (64 classic + 50 trap) | `Wave9090/Source/Presets/`, `Wave9090/Source/DSP/WavetableBank.h` |
| **Plugg VA** | 512 programs | `PluggVA/Source/Presets/FactoryPresets.cpp` |
| **SP-1200** | 512 kit templates | `SP1200/Source/Presets/FactoryProgramBank.cpp` |

## Agent workflow (Preset Agent)

1. Edit **curated seeds** in `FactoryPresets.cpp` or JD category builders.
2. **Daily autonomous batch** (no prompt): `.github/workflows/vst-preset-daily-batch.yml` → `ActiveAgentSeeds.cpp` via lineage DB (`data/vst-preset-lineage/`).
3. Procedural fill to `kFactoryPresetCount` (512) stays **deterministic** (same index → same params).
4. **SP-1200:** static kits only — not in daily batch.
5. Verify counts:

```bash
python3 scripts/vst/verify_factory_preset_counts.py
ctest --test-dir build -R "PluggVA|Wave9090|SP1200" --output-on-failure
```

Hardware SysEx batches remain **`hardware-preset-designer`** (`tools/synth-forge/`).

## Related

- [PluggVA/Docs/PRODUCT_SPEC.md](../PluggVA/Docs/PRODUCT_SPEC.md)
- [Wave9090/Docs/PRODUCT_SPEC.md](../Wave9090/Docs/PRODUCT_SPEC.md)
