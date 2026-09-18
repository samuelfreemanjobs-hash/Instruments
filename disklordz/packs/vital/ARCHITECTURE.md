# Vital preset pack

**Primary implementer:** Claude Code.  
**Target:** [Vital](https://vitalsynth.com/) only — **not** a JUCE build in this repo.

## Purpose

Preset content for the **Vital VST/AU/CLAP** synth: `.vital` patches and optional `.vitalbank` exports. Independent product from the **SP-1200 VST** plugin in [SP1200/](../../../SP1200/).

## Layout

```text
presets/          # .vital files
banks/            # .vitalbank (optional)
pack.manifest.json
PRESET_INDEX.md   # add when presets land
```

## QA

- Open each preset in Vital 1.5+  
- Note macros and categories in `PRESET_INDEX.md`  
- No misleading “official Vital” branding unless licensed  

## Registry

[docs/agent-registry.json](../../../docs/agent-registry.json) · branch: `claude/vital-presets-<suffix>`
