# Handshake — Hardware Preset Designer × VST Preset Agent

| Lane | Store | Agent |
|------|-------|-------|
| Hardware SysEx / SynthForge | `tools/synth-forge/synth_memory.db` | `hardware-preset-designer` |
| JUCE in-plugin banks | `data/vst-preset-lineage/presets.sqlite` | `vst-preset-agent` |

Do not cross-write: VST factory seeds stay in `Wave9090/` / `PluggVA/`; hardware batches stay in SynthForge adapters.
