# MpcSh101 — architecture (APC Plan)

## Core components

| Component | Role |
|-----------|------|
| `Sh101Voice` | Mono note state, legato, glide target |
| VCO | `dsp::Oscillator` saw or pulse; PWM via width or LFO |
| Sub | Fixed −1 oct square (second osc at half frequency) |
| VCF | `StateVariableTPTFilter` lowpass (12 dB character) or 2-pole |
| VCA/VCF EG | Separate `ADSR` for VCA and VCF (or shared attack on punch) |
| Glide | Portamento on frequency |

## Program chain

```text
MIDI mono → glide → current Hz
VCO(saw|pulse, width + pw_lfo) + sub(−1 oct) → mix
→ VCF(cutoff + vcfEG*amt, res)
→ × vcaEG → mono
→ stereo duplicate
```

**Snappy envelopes:** defaults use **short attack (1–3 ms)** and **moderate decay**; acid programs shorten decay further in preset table.

## Surface

Headless APVTS; program change loads envelope + sub defaults.

## Targets

VST3 x86_64 + aarch64; verify with `verify-mpc-sample-vst3.sh`.