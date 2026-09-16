# Phase 5 — suggested follow-ons

Phase 1–4 on branch `cursor/jd-synth-phase1-3015` cover a playable JD-style instrument with clean-room assets. Items below are optional product/engineering tracks.

## Sound & DSP

- ~~Per-tone TVF cutoff/resonance in APVTS~~ (done: `toneNFilterCutoff/Resonance`, **F-Link**)
- ~~LFO1 + pitch envelope from SysEx~~ (per-tone decode on import); tone coupling from JD patch common (JD-800 has fixed arch — plugin-only coupling)
- ~~SIMD for coupled voice render paths~~ — passive tone pairs render with `addBuffers`; pool mix SIMD
- ~~Group A/B block switches + chorus~~ (`groupAEnable`, `groupBEnable`, `groupBChorus`); full JD multi-block chains still simplified

## UI

- Full JD-style slider surface (palette-linked multi-edit)
- ROM browser with waveform preview
- MPE / per-note expression

## Content & legal

- Hand-tuned factory programs beyond procedural `buildPatch()`
- Installer policy: **only** ship embedded `JDUPGROM`; no user dump pipeline in retail build
- ~~Committed golden WAV + CI~~ (`tests/golden/program0-c4-0.5s.wav`); expand suite / hardware reference clips later

## Integration

- CLAP / AU if needed beyond VST3
- Preset bank import/export (beyond SysEx and internal `JDPR` stub)
