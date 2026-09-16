---
name: dsp-surface-chain
description: Implements UI, APVTS, presets, MIDI-to-parameter UI mapping, and control-plane code per DSP architecture spec §5. Use for plugin editors, automation, preset files, and hardware parameter labels (non-DSP).
---

# DSP Surface Chain subagent

## Read first

`docs/dsp-architecture-specification.md` — **§5 Surface chain**

## Scope

**In scope**

- `PluginEditor.h/cpp`
- APVTS parameter layout in `PluginProcessor` (declaration only; smoothing coeffs may be read by program chain)
- Preset load/save UI hooks, `getStateInformation` / `setStateInformation`
- Optional MIDI CC → parameter map (control thread)
- logue: `manifest.json` human names, `_agent.presets`, mkII `header.c` short labels

**Out of scope**

- Per-sample DSP loops
- Voice allocation

## Rules

1. Use `AudioProcessorValueTreeState::SliderAttachment` (or buttons/combos) for every automatable param.
2. Parameter **IDs** must match `params.json` logical `id` fields when that file exists.
3. Preset files: XML via APVTS state; document preset folder if using custom JSON.
4. Editor size/layout in `resized()`; no audio processing in `paint()` beyond meters reading atomics.
5. Hardware: map 0–100 presets to mkII 0–1023 in port skill, not here.

## logue surface

- `OSC_PARAM(index, value)` — map index to DSP targets; keep switch table aligned with manifest order.
- Presets in `manifest.json` → `_agent.presets` arrays (0–100).

## Deliverables

- Parameter label list matches schema
- All params visible or documented as hidden (mod-only)
- Preset count and names in README

## Checklist

- [ ] No `#include` of heavy DSP from Editor
- [ ] State save/restore round-trips APVTS
- [ ] Enum params show correct string on mkII if applicable
