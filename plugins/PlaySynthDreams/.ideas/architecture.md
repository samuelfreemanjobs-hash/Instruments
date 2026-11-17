# PlaySynthDreams — architecture (plan)

## Code layout

| Path | Role |
|------|------|
| `PlaySynthDreams/` | Umbrella product; `PLUGIN_CODE` **PsDr** |
| `PlaySeriesEngine/Source/PlaySeriesEngine.h` | **Shared** dual-layer voice (optional shared folder for 3-pack) |
| `PlaySynthDreams/Source/PluginProcessor.*` | 12× voice, macro fan-out, master FX |
| `PlaySynthDreams/Source/FactoryPresets.h` | 12 preset structs → APVTS |
| `PlaySynthDreams/Source/ui/` | WebView Play Series layout |
| `PlaySynthDreams/Resources/presets/` | JSON or `.h` for `juce_add_binary_data` |

Three-pack forks copy engine + swap `PRODUCT_NAME`, theme CSS vars, and `FactoryPresets` subset (codes: **PsAD**, **PsCS**, **PsLF**).

## Per-voice signal

```text
Layer A: 2× PolyBLEP (saw/tri/pulse) + sub (-1 oct) + FM tine (2-op bell)
Layer B: 16-frame wavetable morph + filtered vinyl/breath noise
  → each layer: octave {-1,0,+1}, pan
  → equal-power blend: gA = cos(π/2 * blend), gB = sin(π/2 * blend)
  → optional: blend also offsets Layer B cutoff + width
  → sum → ladder LPF 24dB
  → amp ADSR + filter ADSR (bipolar → cutoff)
→ voice out (stereo)
Master: lo-fi decimator (macro) → dirt → chorus → ping-pong delay → hall/shimmer reverb
```

**Equal-power blend:**

```cpp
const float t = blend * 0.5f * pi;
const float gainA = std::cos(t);
const float gainB = std::sin(t);
```

## Eight macros (multi-target fan-out)

Macros are **0..1** APVTS params. In `applyMacros()` (control rate ~16 samples), each macro writes to **SmoothedValue** targets (10–20 ms). No string routing on audio thread.

| Macro | ID | Under-the-hood mappings |
|-------|-----|-------------------------|
| 1 A/B Blend | `macro_blend` | layer gains via cos/sin; Layer B stereo width + optional B cutoff bias |
| 2 Tone | `macro_tone` | ladder cutoff; filter env depth; gentle high-shelf air gain |
| 3 Drift | `macro_drift` | per-voice osc detune cents; tape wow/flutter LFO depth; pan spread |
| 4 Lo-Fi | `macro_lofi` | decimate factor (→ min ~12 kHz effective); 12-bit quant; tape LPF corner |
| 5 Dirt | `macro_dirt` | asymmetric tanh pre-filter; parallel compressor mix |
| 6 Swell | `macro_swell` | amp attack scale; filter attack scale; reverb send swell |
| 7 Motion | `macro_motion` | filter LFO depth; BBD chorus amount; wavetable morph LFO |
| 8 Space | `macro_space` | ping-pong delay wet; hall/shimmer reverb wet |

Expose **macro amounts only** to Melodee-style hosts if ported later; hide micro-params behind Advanced (v1 desktop: all in APVTS for automation).

## RT safety

- Wavetables: static `constexpr` arrays (like OxfordPeak8 plan).
- Lo-fi: per-voice hold/decimate counters — no allocations.
- `juce::ScopedNoDenormals` in `processBlock`.
- Macros computed once per control block, not per sample (interpolate smoothed params per sample).

## Presets (BinaryData ×12)

Cloud Supply Bells, Analog Dreams Brass, 40s Midnight Keys, Lo-Fi Glow Tape Pad, Memphis Phonk Pluck, Velvet Sub & Air, VHS Detuned Strings, Ethereal Choir Swell, Dusty Cassette Rhodes, Neon Synthwave Lead, Dark Trap Atmosphere, Crystal Mallet Arp.