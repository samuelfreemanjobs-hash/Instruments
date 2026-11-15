# PlaySynth 6-pack — factory build contract (v2)

Inject this block into every Claude Code / Cursor overnight prompt for the six SKUs.

## Performance (MIDI thread / block edge only — not in `renderNextBlock` alloc)

- Tempo-sync arp: Up, Down, UpDown, Random; 1/8 and 1/16; gate 0.1–1.0
- Chord mode: Off, Octave, Minor7, Minor9, DarkTriad (transpose held notes before voice alloc)
- Legato glide (phonk SKU: emphasize mono glide on `PhonkCowbell94`)

## Pro UX

- XY pad: `xy_x` = A/B blend, `xy_y` = cutoff + space (per-plugin table in `PLAY_SYNTH_SHARED_UI.md`)
- Dice: bounded randomize on message thread only
- Macro lock: when `macro_lock` true, preset changes keep `macro1`…`macro8`
- Velocity → macro depth: optional scale macro 5–6 by note velocity (0–1), control-rate only

## DSP quality

- Dirt/wavefold/cowbell FM paths through `juce::dsp::Oversampling<float>` **2x** prepared in `prepareToPlay`
- Macro fan-out at 16-sample control rate; `SmoothedValue` on cutoff/pitch/gain

## UI (WebView)

- Theme per `play_synth_6pack_master.json` `theme` field
- Lock-free scope: mono sum peak ring buffer (`AbstractFifo`, 4096 samples) written in `processBlock`; WebView polls at ~30 Hz (UI thread only)
- Do not block audio thread on canvas

## State

- APVTS XML via `getStateInformation` / `setStateInformation`
- 12 factory programs in `FactoryPresets.h` + `juce_add_binary_data` JSON under `Resources/presets/`

## References

- `AGENTS.md`, `docs/JUCE8_VST3_PROJECT_RULES.md`
- `plugins/PLAY_SYNTH_6PACK.md`
- APC: `plugins/<Name>/` · Code: repo root `<build_root>/`