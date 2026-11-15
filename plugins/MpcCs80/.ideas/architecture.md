# MpcCs80 — architecture (APC Plan)

## How to code it in JUCE

The CS-80 is a **dual-layer** instrument: every note can drive **two parallel synthesis chains** (Layer I and Layer II). In JUCE, model one **`Cs80Voice`** with two nested **`Cs80Layer`** structs — not two separate plugins.

### Voice graph (one MIDI note)

```text
                    ┌── Cs80Layer I ── HPF ── LPF ──× env ──┐
MIDI note on ───────┤                                         ├── sum → voice out
  + poly AT         └── Cs80Layer II ─ HPF ── LPF ──× env ──┘
```

Each **Cs80Layer**:

```text
VCO1 (saw/pulse + PWM) ──┐
VCO2 (saw/pulse + PWM) ──┼── mixer (+ detune) ── optional RING (v1*v2)
                         └── SUB (square ÷2 from VCO1) ────────┘
```

### Reference C++ shape (program chain)

```cpp
struct Cs80Layer {
  float phase1{}, phase2{}, subPhase{}, lfoPhase{};
  juce::dsp::StateVariableTPTFilter<float> hpf, lpf; // or IIR 12 dB

  float processSample(float sr, float freq, const LayerParams& p,
                      float filterCutoff, float resonance, float env) {
    const float pwm1 = pulseWidthFromLfo(p);
    const float o1 = oscPulse(phase1, pwm1) * (1.0f - p.pulseBlend)
                   + oscSaw(phase1) * p.pulseBlend;
    const float o2 = /* same for VCO2 with detune */;
    const float ring = o1 * o2;
    const float sub = signFromPhase(subPhase);
    const float dry = o1 * p.mix1 + o2 * p.mix2 + ring * p.ringAmt + sub * p.subLvl;

    hpf.setCutoffFrequency(p.hpfHz);
    lpf.setCutoffFrequency(filterCutoff + env * p.envAmt);
    lpf.setResonance(resonance);
    return lpf.processSample(0, hpf.processSample(0, dry));
  }
};

struct Cs80Voice {
  Cs80Layer layerI, layerII;
  juce::ADSR ampEnv, filterEnv;

  float render(..., float layerBalance, float aftertouch) {
    const float cut = baseCutoff + aftertouch * aftertouchAmt;
    const float fEnv = filterEnv.getNextSample();
    const float a = layerI.processSample(..., cut, res, fEnv);
    const float b = layerII.processSample(..., cut * 0.92f, res * 0.9f, fEnv);
    return (a * (1.0f - layerBalance) + b * layerBalance) * ampEnv.getNextSample();
  }
};
```

**PWM:** advance `lfoPhase` per sample; map LFO to pulse width (narrow toward 5% at extreme). Use band-limited polyBLEP or minimum-phase saw/pulse from `juce::dsp::Oscillator` if CPU allows.

**Ring mod:** multiply **after** pitch stabilization, **before** filters (CS topology approximation).

**Poly aftertouch:** in `processBlock`, on each sample iterate active voices; add `(midiPolyPressure[key] - 0.5f) * aftertouchAmt` to that voice’s cutoff only (store `lastPressure` per voice index).

**Voice manager:** `std::array<Cs80Voice, kMaxVoices>` (v1: `kMaxVoices = 4`); steal oldest on overflow; `prepareToPlay` calls `layer.hpf.prepare(spec)` etc. once.

### Processor layout

| File | Role |
|------|------|
| `Source/Cs80Layer.h` | PWM oscs, sub, ring, HPF/LPF per layer |
| `Source/Cs80Voice.h` | Dual layer + ADSR + aftertouch offset |
| `Source/Cs80VoiceManager.h` | MIDI, voice steal, glide |
| `Source/PluginProcessor.cpp` | APVTS, programs, `processBlock` sum |
| `Source/FactoryPrograms.h` | Brass, strings, AT lead snapshots |

### RT rules (MPC Sample)

- No alloc in `processBlock`; voices statically allocated.
- Smooth cutoff / resonance (`SmoothedValue` or one-pole) at block rate, not per param read per sample where avoidable.
- Profile **4 dual-layer voices** on aarch64 before raising voice count.

### Shared code

Reuse patterns from `vst/MpcAnalogSynth` (voice array, headless APVTS) and filter coefficient updates from `MpcVoyager` / `MpcMessenger` (JUCE 7 filter `process` on sub-blocks if needed).

### Melodee mapping

`params.json` → `cutoff`, `res`, `layerBalance` as macro order; programs select brass-heavy vs. string-heavy layer PWM/ring defaults.