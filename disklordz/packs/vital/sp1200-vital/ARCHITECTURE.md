# SP-1200 Vital preset pack

**Primary implementer:** Claude Code.  
**Not a JUCE target** — Vital `.vital` / `.vitalbank` content only.

## Purpose

Companion preset pack for [Matt Tytel Vital](https://vitalsynth.com/) aligned with the SP-1200 VST story (grit, drums, classic sampler tone). Optional future Disklordz storefront SKU.

## Layout

```text
presets/          # .vital files (Claude adds)
banks/            # .vitalbank exports (optional)
pack.manifest.json
```

## QA

- Load each preset in Vital 1.5+  
- Document macro names in `PRESET_INDEX.md` when presets land  
- No copyrighted sample names in preset titles without clearance  

## Coordination

- Plugin DSP: [SP1200/](../../../SP1200/) (Claude, separate PR acceptable)  
- Registry: [docs/agent-registry.json](../../../docs/agent-registry.json)
