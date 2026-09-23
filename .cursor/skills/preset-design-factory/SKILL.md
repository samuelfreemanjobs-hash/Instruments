---
name: preset-design-factory
description: "Design factory presets and programs for JD Upgraded, Wave909, and HISE XML. Use for sound-design SKUs tied to plugin parameters."
---

# Preset design factory

## JD Upgraded

- Read [docs/PRESETS.md](../../../docs/PRESETS.md), [docs/UI.md](../../../docs/UI.md)
- APVTS layout in plugin `createParameterLayout()` — see [docs/ARCHITECTURE.md](../../../docs/ARCHITECTURE.md)
- Factory programs: `FactoryPatchLibrary`, multisample sets 1–24 per [docs/ROM.md](../../../docs/ROM.md)
- SYSEX constraints: [docs/SYSEX.md](../../../docs/SYSEX.md)

## Wave909

- [Wave909/ARCHITECTURE.md](../../../Wave909/ARCHITECTURE.md) — wavetable / trap voice design

## HISE

- Do not run HISE in Cloud VM. Write XmlPresetBackups + scripts; send via Antigravity bridge skill.

## Sound design discipline

- Name presets with SKU + version (`DL006_PhonkBell_v1`).
- Document macro intent (attack, filter, saturation) in preset README or Airtable row.
- After DSP-affecting preset defaults in C++, run `juce-dsp-plugin-ci` skill.
