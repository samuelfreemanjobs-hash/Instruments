# Phase 5 — suggested follow-ons

Phase 1–4 on branch `cursor/jd-synth-phase1-3015` cover a playable JD-style instrument with clean-room assets. Items below are optional product/engineering tracks.

## Sound & DSP

- ~~Per-tone TVF cutoff/resonance in APVTS~~ (done: `toneNFilterCutoff/Resonance`, **F-Link**)
- ~~LFO1 + pitch envelope from SysEx~~ (per-tone decode on import); tone coupling from JD patch common (JD-800 has fixed arch — plugin-only coupling)
- ~~SIMD for coupled voice render paths~~ — passive tone pairs render with `addBuffers`; pool mix SIMD
- ~~Group A/B block switches + chorus~~ (`groupAEnable`, `groupBEnable`, `groupBChorus`); full JD multi-block chains still simplified

## UI

- ~~Palette multi-edit~~ (**P-Link** applies category snap to all tones); full JD slider surface still open
- ~~ROM browser with waveform preview~~ (`WavePreviewComponent`, tone A–D + wave slider)
- ~~Basic expression~~ (pitch wheel, channel/poly aftertouch → filter; `expressionDepth`)

## Content & legal

- ~~Curated factory overrides~~ (programs 0, 48, 112); more hand-tuning optional
- ~~Installer policy doc~~ ([INSTALLER_POLICY.md](INSTALLER_POLICY.md))
- ~~Golden WAV suite + CI~~ (`tests/golden/manifest.tsv`, 6 factory programs); hardware reference clips later

## Integration

- CLAP / AU — documented in [INTEGRATION.md](INTEGRATION.md); blocked on JUCE CLAP target / macOS AU build
- ~~APVTS preset `.jdpreset` import/export~~ (editor + XML); full bank / JDPR expansion optional
