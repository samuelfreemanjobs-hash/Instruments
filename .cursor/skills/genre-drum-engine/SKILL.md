---
name: genre-drum-engine
description: >-
  Adds a portable genre/producer drum engine to Drum Design Lab MultiGenre
  (KitStyle, MIDI map, pattern helper, tests, batch WAVs). Use when adding
  Tay Keith, Toomp, Purps, Shawty, Hitmaker-style modes or any new KitStyle.
---

# Genre Drum Engine

## Pattern (copy from nearest sibling)

1. Create `vst/DrumDesignLab/Source/DSP/<Name>Engine.h`
   - Namespace `disklordz::<short>`
   - Params POD + presets + `VoiceID` + `PatternHit` helper
   - `prepare` / `reset` / `setParams` / `handleMidiNote` / `renderTick` / `renderOneShot`
   - Use `CoreMathAndPRNG.h` (`TptSvf`, `DeterministicRng`, `OversampledWaveshaper4x`)
2. Wire `MultiGenreDrumEngine.h`:
   - `#include`
   - `KitStyle::<Name>` before `Count`
   - `kitStyleName` string
   - `prepare` / `reset` / `setStyle` / `handleMidiNote` / `renderTick` member
3. Extend `Tests/m1_core_test.cpp` (peak, NaN, pattern density, choke if hats)
4. Optional: `Tools/render_kit_batch.cpp` one-shot export folder
5. Document row in `ARCHITECTURE.md`

## MIDI map (default)

| Note | Role |
|------|------|
| 36 / 48–72 | 808 (melodic pitch) |
| 37 | Rim / top kick |
| 38 / 40 | Snare |
| 39 | Clap |
| 42 / 44 | Closed hat |
| 46 | Open hat (choke group with closed) |
| 49 / 51 | Crash |

## Differentiation rule

Do **not** only change gain/cutoff. Change pitch envelopes, waveshaper topology, resonator structure, and groove steps.

## Verify

```powershell
# from vst/DrumDesignLab
cmake --build build-core --config Release
ctest --test-dir build-core -C Release --output-on-failure
# Device Guard may block new .exe names — copy to ddl_batch.exe if needed
```

## Forbidden

- JUCE types inside portable engines
- Dynamic alloc in `renderTick`
- Trademarked pack names as product titles (use descriptive non-infringing labels)
