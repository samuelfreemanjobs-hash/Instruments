# ProphetMod16 — architecture (plan)

## Code layout

| Path | Role |
|------|------|
| `ProphetMod16/` | Root CMake target (same pattern as `ProphetRev2Trap/`) |
| `ProphetMod16/Source/ProphetMod16Engine.h` | **Single engine header** — all RT DSP (see `docs/CLAUDE_VST_ENGINEER.md`) |
| `ProphetMod16/Source/PluginProcessor.*` | APVTS, `juce::Synthesiser`, MIDI, `pushParametersToEngine()` |
| `ProphetMod16/Source/PluginEditor.*` | WebView + relays (JUCE 8) |
| `ProphetMod16/Source/ui/` | HTML/CSS/JS (dark slate + wood) |
| `ProphetRev2Trap/Source/ProphetRev2Engine.h` | **Reuse reference** — PolyBLEP-ish osc, slop, 2/4-pole TPT, ADSR |

## Voice signal flow

```text
MIDI → Synthesiser (16× ProphetMod16Voice)
  → dual PolyBLEP osc (saw / tri / saw-tri / PWM) + sub (-1 oct square) + noise
  → osc mix × slop (per-voice pitch + cutoff drift)
  → pre-filter drive (fastTanh)
  → LP 12dB/24dB (Ladder or cascaded SVF)
  → filter env (bipolar → cutoff) + key track + matrix
  → amp env × velocity → pan → voice sum
Master: chorus → stereo delay (tempo) → soft clip limiter
```

## Modulation matrix (RT-safe)

**Do not** use `std::vector` or string IDs on the audio thread.

**Preferred in this repo:** expose **8 slots × 3 params** in APVTS (`modN_src`, `modN_dst`, `modN_amt`). UI writes via attachments; audio thread reads `getRawParameterValue` only (see `docs/JUCE8_VST3_PROJECT_RULES.md`).

Enums (compile-time, shared header):

```cpp
enum class ModSource : int {
    None = 0,
    LFO1, LFO2, LFO3, LFO4,
    FilterEnv, AuxEnv,
    Velocity, ModWheel, Aftertouch, KeyTrack,
    NumSources
};

enum class ModDest : int {
    None = 0,
    Osc1Pitch, Osc2Pitch, AllOscPitch,
    Osc1PW, Osc2PW, OscMix,
    FilterCutoff, FilterResonance,
    LFO1Rate, LFO2Rate,
    VCAlevel, Pan,
    NumDests
};
```

Per voice, each control block:

1. Sample LFOs + envs at **control rate** (every 16 samples default, `kModControlInterval`).
2. Sum matrix contributions into **offset buffers** (fixed `std::array<float, NumDests>` on stack).
3. Apply offsets to **SmoothedValue** base targets; interpolate smoothed pitch/cutoff per sample.

## CPU budget

- 16 voices × (2 osc + sub + noise + filter + 3 env + 4 LFO reads) at 48 kHz: matrix + LFOs at **1/16 audio rate** → ~60%+ savings vs per-sample matrix.
- PolyBLEP only on osc edges; no oversampling in v1.

## Threading

- **Audio:** no alloc, no locks, `ScopedNoDenormals` in `processBlock`.
- **UI:** WebView mod grid → APVTS only.

## Presets

10 factory programs embedded `BinaryData` (names from factory prompt): Init Saw Poly, Rev2 Warm Brass, Evolving Glass Pad, Analog PWM Strings, Deep Sub Bass, Sync Lead, Modulated Bell Keys, Unison Reese, Cosmic Arp Pluck, Dark Cinematic Drone.