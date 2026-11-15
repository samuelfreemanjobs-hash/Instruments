# PlaySynth Trilogy — shared UI + Dice contract

## APVTS (all three plugins)

| ID | Role |
|----|------|
| `macro1` … `macro8` | Plugin-specific macro fan-out |
| `layer_blend` | A/B equal-power (also driven by `xy_x`) |
| `xy_x` | 0..1 blend override |
| `xy_y` | 0..1 cutoff + space axis (per-plugin mapping) |
| `dice_trigger` | Momentary; UI thread only |

## XY pad (WebView → processor)

On drag: set `xy_x`, `xy_y`. Processor maps:

- `effectiveBlend = xy_x` (or macro blend if XY inactive)
- `xy_y` scales ladder cutoff + reverb/delay send per plugin table

## Dice randomizer (message thread)

`randomizePatchBounded(juce::Random&)`:

- Randomize: layer wave/preset indices, filter cutoff (log), macro1–8 within [0.15, 0.85], osc detune small
- **Never** change: `master_level`, output gain, polarity
- Clamp peak by previewing macro Dirt/Warmth caps
- Push APVTS via `setValueNotifyingHost`

No randomization from audio thread.