# MpcMinimoog — architecture (APC Plan)

**Normative:** [docs/dsp-architecture-specification.md](../../../docs/dsp-architecture-specification.md) · [docs/MPC_SAMPLE_VST3.md](../../../docs/MPC_SAMPLE_VST3.md)

## Core components

| Component | Location | Role |
|-----------|----------|------|
| `MonoVoice` | `Source/Dsp/MinimoogVoice.h` | Single active note; legato retrigger rules |
| `OscBank` | same | 3× `dsp::Oscillator` + optional micro **drift** (± few cents, sub-Hz LFO per osc) |
| `NoiseGen` | same | White noise, level mix |
| `Glide` | `Source/Dsp/Glide.h` | Exponential portamento on **master pitch** (MIDI Hz target) |
| `Mixer` | inline | osc1+2+3+noise → pre-filter |
| `LadderFilter` | `juce::dsp::LadderFilter<float>` | 4-pole LP; **drive** pre- or post-filter per design |
| `Envelopes` | `ADSR` amp + `ADSR` filter (shared or split) | Filter contour from `filter_eg_amt` |
| Processor | `PluginProcessor` | MIDI mono allocator; last-note priority |

## Program chain

```text
MIDI (mono, legato optional):
  note target → Glide (glide time) → current pitch Hz
per sample (or per small sub-block):
  osc1,2,3 (+drift) + noise → mix × levels
  → soft clip/drive (drive param)
  → LadderFilter(cutoff + filterEG*amt, emphasis)
  × ampADSR → mono sample
duplicate to L/R (or subtle width from osc2 detune pan — v2)
```

**RT:** all buffers in `prepareToPlay`; glide state = 2 floats; no voice pool allocation.

## Surface chain

- Headless APVTS; programs switch full parameter sets + glide defaults.
- `hasEditor() == false`.

## Global

- **One voice** always; new note steals or legato per `legato` param (design phase).
- Targets: x86_64 + aarch64 VST3.

## CPU note

Mono allows **per-sample** ladder on ARM if block size 128; if hot, move to 4-sample inner loop with same coeffs per mini-block.