# SemV — architecture (APC Plan)

**Reference:** Arturia SEM V · **Normative:** `docs/dsp-architecture-specification.md`

## Global layer

| Concern | Implementation |
|---------|----------------|
| Sample rate | `prepareToPlay` → `SemContext::{sr, maxBlock}` |
| Voice pool | Fixed **8** voices (configurable 4–16 via `voices` at prepare; no runtime alloc) |
| Presets | APVTS + `FactoryPresets.h` + optional `Resources/presets/*.json` |
| Tuning / bend | Global cents + per-note bend in `MidiKeyboardState` |
| Plugin codes | `PLUGIN_MANUFACTURER_CODE Inst`, `PLUGIN_CODE SemV` (confirm 4-char in impl) |

## Program chain (audio thread)

Ordered signal path per voice, summed poly:

```
MIDI note on/off
  → SemVoiceManager (fixed pool, steal oldest)
  → SemOscPair (saw + pulse PW, detune, sub −1 oct, optional noise)
  → SemStateVariableFilter (TPT SVF: LP / BP / HP / notch morph)
       ← SemAdsr amp_env, filter_env (key track on cutoff)
       ← LfoMod (PW / cutoff / amp, control-rate coeffs updated block edge)
  → voice out (stereo pan optional v2)
→ SemMixer (velocity, master smooth)
→ SemFxBus (chorus LFO delay taps → feedback delay → Schroeder plate lite)
→ output trim
```

### Key modules (`vst/SemV/Source/Dsp/`)

| Module | Role |
|--------|------|
| `SemOscPair.h` | PolyBLEP saw/pulse, sub square, white noise |
| `SemStateVariableFilter.h` | 12 dB SVF; mode as continuous morph or 4-way enum |
| `SemAdsr.h` | Shared envelope helper; exponential segments |
| `SemLfo.h` | Sine/tri; block-edge rate sync from host PPQ |
| `SemVoice.h` | `SynthesiserVoice` subclass; no heap in `renderNextBlock` |
| `SemEngine.h` | Owns voice array, FX bus, MIDI fan-in |
| `SemFxBus.h` | Stereo chorus (mod delay) + delay line + cheap reverb |

**RT rules:** `SmoothParam` (or `juce::SmoothedValue`) on cutoff, levels, FX mix; no locks/logging on audio thread.

## Surface chain (control plane)

| Surface | Role |
|---------|------|
| APVTS | All IDs from `parameter-spec.md`; smoothing 20–50 ms on cutoff/reso |
| `PluginProcessor` | MIDI, state save, program change (12 factory) |
| `PluginEditor` | **JUCE native** — Arturia-like sections: Osc / Filter / Env / LFO / FX |
| MIDI CC | Default map: CC74 cutoff, CC1 mod wheel → `filt_env_amt` or LFO depth |

## Parameter mapping (surface → program)

| APVTS group | Program destinations |
|-------------|---------------------|
| Osc\* | `SemOscPair` levels, waves, PW, detune, sub, noise |
| `filter_*` | SVF cutoff (Hz), reso, mode, env depth, key track |
| `amp_*` / `filt_*` | ADSR targets |
| `lfo_*` | Mod matrix (single dest v1) |
| `chorus_*` / `delay_*` / `reverb_*` | `SemFxBus` wet/dry |
| `master` | Output gain after FX |

## Build & test

| Target | Command |
|--------|---------|
| VST3 + Standalone | `./tools/build-vst.sh SemV` (after `/apc-impl`) |
| CI DSP | `vst-testing-ops/run_business.py --profile factory --plugin SemV` |
| Golden | 2–3 offline notes (brass sweep, notch lead) → `tests/golden/` when stable |

## Extension points

- v1.1: second LFO, oscillator sync, unison ×2 (raises tier)
- MPC port: `MpcSemV` headless, K1–K3 = cutoff / reso / chorus (separate APC)
- Params manifest: `plugins/SemV/params.json` at design phase for host docs