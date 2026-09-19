# Quality control validation

## Content validation

- Sample / multisample / tone / patch IDs exist and are unique
- Manifest valid; `engineVersion` / `schemaVersion` compatible
- No duplicate IDs in package
- Expansion `contentId` authorized (when license system exists)

## Audio validation

- No clipping on import (warn/error thresholds)
- No NaN/Inf in PCM
- DC offset within tolerance
- Sample rate supported (48 kHz default)
- Loop points inside file; loop length minimum
- Root note in 0–127; velocity ranges valid

## Musical validation (automated smoke)

- MIDI note on/off
- Velocity layers audible (where defined)
- Pitch bend, sustain
- Macros move parameters
- 1 s render peak > silence threshold

## Plugin validation

- Loads in host; state save/restore
- Pluginval / CI harness
- DAW matrix (Ableton, FL, Reaper, Logic, …) — manual release gate

## Tooling

`DisklordzSynth_RomFactory validate-package <dir>` — runs content + manifest checks (expand over time).

Rompler: `ContentValidation.cpp` for ID resolution when patch JSON loads.
