# VOYAGER-V — UI concept (source of truth for editor)

Design reference:

![VOYAGER-V panel concept](UI_CONCEPT_VOYAGER-V.png)

## Branding

| Element | Concept |
|---------|---------|
| Product name | **VOYAGER-V** |
| Manufacturer line | **FOUR-STACK MOOG ENGINE · SYSTEMS ENGINEERING AUDIO** |
| **Moog stack** | Four **STACK MODE** switches + lineage badge + accent color |
| Patch line | `PATCH: PROG nn "Name"` · **&lt; / &gt;** preset step buttons |

## Panel zones → current APVTS

| Zone | Concept controls | V Voyager today | Phase |
|------|------------------|-----------------|-------|
| **Oscillator 1–3** | Wave, frequency, fine, mix faders, sync | 3× **mix faders**, fine, wave, octave; mixer drive | **Phase 2** ✓ mix · sync Phase 3 |
| **Noise** | White/pink | Noise level | **MVP** ✓ · pink Phase 2 |
| **Filter** | Large cutoff, res, drive, key track, overdrive switches | Cutoff, res, drive, env amt, KB, mixer sat | **MVP** ✓ · oversized cutoff Phase 1 |
| **Filter / Amp env** | ADSR sliders | Dual **vertical ADSR** rows (FILTER / AMP) | **Phase 2** ✓ |
| **Mod bus** | LFO, wheels, routing | LFO rate, → pitch/filter, glide | Phase 2 (wheels = host MIDI) |
| **Output** | Master, headphone, meter | Master gain | Phase 2 meter/HP |
| **Header** | Oversampling, CPU, meter | — | Phase 3 |
| **Performance** | Keyboard, pitch/mod wheels, X-Y pad | Host MIDI only | Phase 3 (optional mini keyboard) |
| **Visualizer** | Scope | — | Phase 3 |

## Implementation status

- **Phase 1 (this repo):** VOYAGER-V header, **keyboard lineage** selector, patch readout, wood side rails, metal panel, prominent cutoff knob, sub osc, factory preset browser. See [MOOG_FOUR_STACK.md](MOOG_FOUR_STACK.md).
- **Phase 2 (in progress):** vertical ADSR sliders, osc mix faders, preset prev/next. Remaining: filter/OD switches, mod bus faders/labels.
- **Phase 3:** Scope, oversampling toggle, touch surface / wheels (or host-only note).

When adding parameters, update [PRESETS.md](../PRESETS.md) generator and `SynthParameterIds.h` together.
