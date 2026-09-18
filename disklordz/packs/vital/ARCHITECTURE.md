# Pluggnb Vital preset pack

**First Instruments preset SKU.** **Primary implementer:** Claude Code.  
**Target:** [Vital](https://vitalsynth.com/) only (`.vital` / `.vitalbank`) — not a JUCE plugin in this repo.

## Purpose

Preset bank for **Pluggnb** production in Vital: bright/wide bells, keys, pads, plucks, and bass/lead tones that sit with sliding 808s. Separate product from the **SP-1200 VST** in [SP1200/](../../../SP1200/).

## Product brief

[Docs/PRODUCT_SPEC.md](Docs/PRODUCT_SPEC.md)

## Layout

```text
presets/              # .vital files by category
banks/                # optional .vitalbank export
pack.manifest.json
Docs/PRODUCT_SPEC.md
PRESET_INDEX.md       # preset list + macros (when shipped)
```

## QA

- Load in Vital 1.5+ on 44.1 / 48 kHz  
- A/B with reference Pluggnb beats (808 + melody balance)  
- Document each preset in `PRESET_INDEX.md`  
- Do not imply official Vital endorsement  

## Registry

[docs/agent-registry.json](../../../docs/agent-registry.json) · branch: `claude/pluggnb-vital-<suffix>`
