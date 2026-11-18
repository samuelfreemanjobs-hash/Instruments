---
name: drum-voice-recipes
description: Formulate original synthesized kick, 808, cowbell, snare, clap, hat, perc and texture recipes with deterministic voice parameters
---
# DRUMSMITH voice design
For each voice specify oscillator/noise source, pitch/time envelope, amplitude envelope, filter stages, saturation curves, gain staging, key/root, tonal confidence, oversampling requirements, seed and render headroom. Treat defaults as [HYPOTHESIS], not calibration data.

## Starting hypotheses (tune by audition)
- Drift 808: sine/triangle core; initial pitch 120–180 Hz falling to 40–60 Hz within 25–80 ms; 0.3–1.5 s tail; asymmetric saturation with strict DC removal.
- Memphis cowbell: two inharmonic band-limited square-like oscillators near 540/800 Hz; 60–350 ms decay; light bandpass/drive; pitch labeling may be approximate due to inharmonic partials.
- Trap snare: noise + pitched body ~170–250 Hz; fast transient; short 150–450 ms tail; saturation 5–30% wet and optionally very short gated room.
- Racing hats: filtered noise/metallic oscillators, 20–200 ms decay, velocity/pitch/randomness derived from seed, avoid cumulative DC.
- Percussion: modal partials or filtered noise; tag `pitched` only when reliable tuning is measured.

## Real-time DSP handoff
No allocations/locks/IO/console in audio callback; preallocate voices and delay lines; parameter smoothing; denormal avoidance; deterministic note-on reset; bounds checked; offline oversample or band-limit waves before downsampling; document audible aliasing risks at high pitches and distortion stages. Validate both 44.1 and 48 kHz and multiple buffer sizes.
