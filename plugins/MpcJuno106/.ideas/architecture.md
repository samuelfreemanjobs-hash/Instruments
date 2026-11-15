# MpcJuno106 — architecture (APC Plan)

**Normative:** [docs/dsp-architecture-specification.md](../../../docs/dsp-architecture-specification.md) · [docs/MPC_SAMPLE_VST3.md](../../../docs/MPC_SAMPLE_VST3.md)

## Core components

| Component | Location | Role |
|-----------|----------|------|
| Context | `PluginProcessor` | `sampleRate`, block size; voice pool size `kMaxVoices` |
| ParameterBank | APVTS | IDs from parameter-spec v1; cutoff/res/chorus read every block |
| Voice pool | `Source/Dsp/JunoVoice.h` | Fixed 6–8 `JunoVoice` instances |
| DCO | `Source/Dsp/JunoDco.h` | Saw (DPW or polyBLEP), pulse PWM, sub −1 oct |
| Filter | `Source/Dsp/JunoLpf24.h` | 4× 1-pole or TPT 24 dB LP; resonance 0–1 |
| Amp env | `juce::ADSR` per voice | Standard ADSR |
| Chorus | `Source/Dsp/JunoChorus.h` | Stereo modulated delay taps (BBD-ish, **post-voice sum**) |
| Presets | `ProgramBank` in processor | 4+ factory programs |

## Program chain

```text
MIDI → VoiceAllocator (fixed pool)
per active voice per block:
  DCO (saw + pulse*w + sub) → sum
  → LPF 24 dB (cutoff, res)
  → ADSR → voice out (mono)
sum voices → stereo (duplicate or slight pan law)
→ JunoChorus(cutoff-adjacent send, chorus mix) → output
```

**RT rules:** no alloc in `processBlock`; chorus delay lines sized in `prepareToPlay`; LPF coefficients updated from smoothed cutoff (10–20 ms).

## Surface chain

- **Melodee:** `hasEditor() == false`; host programs for preset change.
- APVTS attachments none (headless); state XML + program index in `getStateInformation`.

## Global

- Sample rates: 44.1 / 48 kHz (MPC Sample reference 44.1).
- Targets: `VST3` x86_64-linux + **aarch64-linux** bundle.