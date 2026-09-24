# Night Circuit — product plan

**Product name:** Night Circuit · **v0.1** shipped in tree  
**CMake target:** `ProphetRev2Trap`

## v0.1 scope (complete)

- 16-voice poly + **mono mode** with **legato glide**
- **Analog circuit:** ladder LPF24, filter input sat, output soft clip, key-tracked cutoff
- **Unison** 1–3 voices, spread
- **Five factory categories:** Bass, Synth, Lead, Pad, Pluck (+ **User** bank)
- Category + preset dropdowns, role hints
- **User preset** save/load (`UserPresetStore` → `~/Application Data/Instruments/NightCircuit/Presets/`)
- Amp + filter ADSR (UI + APVTS)
- Full osc/filter/analog panel

## Milestone C (next)

| Item | Notes |
|------|--------|
| **CLAP** | Reuse `clap_juce_extensions` from root JD Upgraded |
| **vst-testing-ops** | Add pluginval smoke job for `Night Circuit.vst3` |
| **Preset expand** | 8–12 Synth programs; A&R overlap audit |
| **MPE** | Only if SKU needs it |

## Milestone D (research)

- PWM / third osc / Prophet-style modulation matrix
- Built-in arp (low priority — DAW-first)

## Category taxonomy

See [PRESET_CATEGORIES.md](PRESET_CATEGORIES.md).
