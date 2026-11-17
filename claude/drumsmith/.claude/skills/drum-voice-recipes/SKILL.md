---
name: drum-voice-recipes
description: W2 voice DSP recipes (kick, 808, snare, clap, hats, cowbell, bus)
---

# Skill — drum voice recipes

## Voices

| Voice | Starting points |
|-------|-----------------|
| Kick | Pitch sweep down 80→45 Hz, click @ 3–5 kHz `[A]` |
| 808 | Sine + soft clip; glide 120 ms `@` F1 `[A]` |
| Snare | Tonal 180 Hz + band-limited noise `[S]` MemphisPhonkFactory |
| Clap | 3 bursts, 2–4 ms spacing `[A]` |
| CH/OH | 808 metallic ratios `[S]`; OH longer decay |
| Cowbell | Square + bandpass, F5 detune cents `[A]` |

## Bus

DC blocker → soft clip −1 dBFS → 8-sample fades. Tag oversample ×2 on nonlinear paths `[S]`.

## Output

Markdown tables with parameter, value, unit, tag (S/C/A). FORGE handoff: struct field names only, no UI code.