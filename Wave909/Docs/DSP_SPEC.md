# WAVE-909 — DSP Specification

## Signal flow

```text
MIDI
 ↓
VoiceManager (16 voices)
 ↓
WavetableOsc (+ sub sine)
 ↓
Filter (ZDF 2-pole LP) — optional HP blend in destructive mode
 ↓
Circuit stage (Pristine: gentle tanh / Destructive: drive + decimator)
 ↓
Tape wobble (pitch LFO + velocity-scaled noise)
 ↓
Amp envelope
 ↓
Stereo pan / width
 ↓
Output gain + soft limiter
```

## Wavetable engine

**Decision:** 64 waves × 2048 samples, generated once at `prepareToPlay`.

**Why:** Matches Wave-style scanning without shipping PCM; presets morph via position only.

**Alternatives:** Runtime FFT resynthesis (higher CPU). **Tradeoff:** Fixed bank, extensible later with import.

**Anti-aliasing:** Cubic interpolation + band-limited wave construction (harmonic series capped by table length).

## Filter

**Decision:** Trapezoidal ZDF two-pole lowpass with tanh-limited resonance (JD Upgraded TVF pattern, clean-room reimplementation).

**Destructive mode:** Same filter with higher drive into decimator (sample-hold + bit depth reduction).

## Tape wobble macro

**Decision:** Single macro scales: (1) slow sine pitch LFO, (2) faster flutter LFO, (3) noise floor gain, (4) sub detune cents.

## Oversampling

**Decision:** None on filter in v0.1; destructive path uses pre-decimation soft clip only.

**Why:** CPU budget for 16 voices. Revisit if aliasing audible in QA.

## Voice engine

16 voices, newest-note stealing, mono legato optional via glide parameter (P0: glide time only).
