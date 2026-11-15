# OxfordPeak8 — architecture (plan)

## Layout

| Path | Role |
|------|------|
| `OxfordPeak8/` | Root CMake target; `PLUGIN_CODE` **Opk8** |
| `OxfordPeak8/Source/OxfordPeak8Engine.h` | All RT DSP in one header |
| `OxfordPeak8/Source/WavetableBank.h` | 16 single-cycle frames × 2048 samples, constexpr |
| `OxfordPeak8/Source/PluginProcessor.*` | `juce::Synthesiser`, APVTS, master FX chain |
| `OxfordPeak8/Source/ui/` | Peak-inspired WebView |

## Per-voice graph

```text
3× OxfordOsc (sine/tri/saw/pulse OR wavetable morph + shape)
  + noise (white)
  → osc mixer levels
  → linear FM: osc1→2, osc2→3, osc3→1 (front-panel amounts)
  → pre-filter tanh drive
  → SVF LP/BP/HP 12dB or 24dB + resonance + key track + filter divergence (per-voice cutoff offset)
  → post-filter tanh
  → amp ADSR × velocity
→ voice bus
→ master tanh → chorus → stereo delay → reverb → out
```

## Wavetable / shape

- Phase accumulator per osc; **shape** interpolates wavetable frame index (0..15) and PWM when in classic pulse mode.
- Band-limited read: linear interpolate across table + crossfade adjacent frames (no alloc).

## Triple overdrive

Reuse `AudioUtils::fastTanh` pattern from `ProphetRev2Trap/Source/ProphetRev2Engine.h` for all three stages; amounts from APVTS, smoothed 10–20 ms.

## Dual-source mod matrix (16 slots)

**RT rule:** No `vector`, no string lookup in `renderNextBlock`.

Each slot **N** = 4 APVTS parameters:

| Param | Meaning |
|-------|---------|
| `modN_srcA` | enum ModSource (0 = none → 0 contribution) |
| `modN_srcB` | enum ModSource (**Direct** = constant 1.0f) |
| `modN_dest` | enum ModDest |
| `modN_depth` | bipolar -1..+1 |

Evaluation at **control rate** (every 16 samples):

```cpp
const float a = getSourceValue(srcA, voice, globals);
const float b = (srcB == ModSource::Direct) ? 1.0f : getSourceValue(srcB, voice, globals);
const float mod = a * b * depth;
accumulateDest(dest, mod);
```

Then apply `destAccum` to smoothed base parameters (pitch, wt position, FM, cutoff, res, drive, LFO rate).

**Poly LFOs** (1–2) live per voice; **global LFOs** (3–4) computed once per control block and copied into each voice.

## Envelopes

- Amp ADSR (hardwired VCA).
- Mod env 1 & 2: ADSR, optional loop flag (retrigger from phase counter; no alloc).

## Master FX

- `juce::dsp::Chorus` (3-mode via mix/type param).
- Stereo delay: tempo sync, feedback HF/LF damping in feedback path.
- `juce::dsp::Reverb` lush preset + pre-delay APVTS.

## CPU

8 voices × 3 osc + FM + 16 matrix rows: matrix only at 1/16 sample rate; wavetable at audio rate; FX at block rate where possible.

## Presets (BinaryData ×10)

Init Oxford Saw, Peak Lush Wavetable, 3-Op FM Glass Keys, Driven Analog Bass, Ambient Reverb Pad, Acid Overdrive Lead, Evolving Mod Choir, Brassy Unison Stab, Shimmering Bell Arp, Dark Hybrid Drone.