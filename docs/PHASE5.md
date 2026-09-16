# Phase 5 — suggested follow-ons

Phase 1–4 on branch `cursor/jd-synth-phase1-3015` cover a playable JD-style instrument with clean-room assets. Items below are optional product/engineering tracks.

## Sound & DSP

- Per-tone TVF cutoff/resonance in APVTS (not only global Res)
- LFO and pitch envelope from SysEx; tone coupling from JD patch common
- SIMD for coupled voice render paths
- Deeper Group A/B modeling (block routing vs single drive/reverb macros)

## UI

- Full JD-style slider surface (palette-linked multi-edit)
- ROM browser with waveform preview
- MPE / per-note expression

## Content & legal

- Hand-tuned factory programs beyond procedural `buildPatch()`
- Installer policy: **only** ship embedded `JDUPGROM`; no user dump pipeline in retail build
- External golden WAV suite vs reference hardware (using `OfflineRender` + `SpectralDiff`)

## Integration

- CLAP / AU if needed beyond VST3
- Preset bank import/export (beyond SysEx and internal `JDPR` stub)
