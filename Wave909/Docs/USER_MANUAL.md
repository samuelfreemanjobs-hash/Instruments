# WAVE-909 — User Manual (v0.1.0)

## Installation

After building or receiving a release package, install:

- **VST3:** copy `WAVE-909.vst3` to your system VST3 folder.
- **CLAP:** copy `WAVE-909.clap` to your CLAP folder.
- **Standalone:** run the `WAVE-909` executable.

## Quick start

1. Load **WAVE-909** on a MIDI track.
2. Choose a factory preset from the dropdown (e.g. **Sub-Zero 808**, **Dark Bell**).
3. Sweep **Wavetable** and **Scan** for morphing timbres.
4. Enable **Destructive Circuit** and raise **Drive** / **Crush** for phonk grit.
5. Use **Tape Wobble** for wow, flutter, and noise-floor character.

## Controls

| Control | Purpose |
|---------|---------|
| Wavetable / Scan | Position and animated scan through the 64-wave bank |
| Cutoff / Res | Analog-style lowpass |
| Destructive Circuit | Switches pristine vs saturated/decimated path |
| Drive / Crush | Aggression in destructive mode |
| Tape Wobble | Macro: pitch drift + hiss + sub detune |
| Sub / Detune / Glide | Low end and performance feel |
| Amp / Filter envelopes | ADSR rows at bottom of UI |
| Output | Master level in dB |

## MIDI

Standard note on/off, velocity, pitch bend. Sustain pedal recommended for pads.

## Presets

Factory programs are read-only starting points; tweak and save via your DAW’s plugin state / project.

## Support

Developer architecture: [ARCHITECTURE.md](../ARCHITECTURE.md).
