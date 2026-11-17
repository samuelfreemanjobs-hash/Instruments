# Nebula6 — architecture (APC Plan)

**Normative:** [docs/dsp-architecture-specification.md](../../../docs/dsp-architecture-specification.md) · **Dream:** [creative-brief.md](creative-brief.md) · **Proof:** [sonic-proof.md](sonic-proof.md)

## Global layer

| Concern | Implementation |
|---------|----------------|
| Context | `NebulaContext` — `sampleRate`, `maxBlockSize`, `kMaxVoices` (8 default, 4–12 APVTS at prepare only) |
| Presets | 13 programs + `preset_tag` metadata (`cloud` \| `phonkwave` \| `atmospheric`); `macro_lock` honoured on program change |
| Tuning | `bend_range`, global fine tune; scale lock in `PerformanceEngine` (block edge) |
| Codes | `PLUGIN_MANUFACTURER_CODE Inst`, `PLUGIN_CODE Nb6s` |
| Shared DSP | `shared/dsp/juno/` — **extracted from `MpcJuno106` plan** before Nebula impl; Nebula6 links same objects |
| Params manifest | `plugins/Nebula6/params.json` at design (L2) — IDs from [parameter-spec.md](parameter-spec.md) |

## Program chain (audio thread)

```text
MIDI in
  → PerformanceEngine (block edge): scale_lock, chord_mode, arp_enable, glide_ms
  → NebulaVoiceManager (fixed pool, steal oldest)
  → per voice NebulaVoice:
       JunoDco (saw, pulse PWM, sub, thermal drift coeff)
       → JunoLpf24 (cutoff, res, filt ADSR + env amt)
       → HollowNotchSend (parallel, wet = hollow_notch)
       → Amp ADSR
  → voice sum (mono per voice → pan law stereo)
  → NebulaBus (stereo, keyed from note aggregate / fixed timbre):
       FormantCloud5 (nebula_vowel, nebula_level)
       + CrystalAirFm (fixed 3-partial, low mix)
       + GhostOvertone808 (gated by ghost_overtone & drift macro)
  → equal-power merge(plane_blend): core vs NebulaBus return
  → NebulaChorus (I/II/Drift/Frozen; extends JunoChorus taps)
  → TapeGravity (wow/flutter, soft clip, gentle LPF)
  → ShimmerHalo (short pitch-shifted feedback, mix cap 0.45)
  → master_level (smoothed)
```

### Module map

| Module | Path | Notes |
|--------|------|-------|
| `JunoDco` | `shared/dsp/juno/JunoDco.h` | Shared with MpcJuno106 |
| `JunoLpf24` | `shared/dsp/juno/JunoLpf24.h` | Smoothed cutoff 15–25 ms |
| `JunoChorus` | `shared/dsp/juno/JunoChorus.h` | Base BBD taps |
| `NebulaChorus` | `Nebula6/Source/Dsp/NebulaChorus.h` | Mode enum; **Frozen** latches LFO phase; **Drift** scales rate from `drift_couple` + tape wow state |
| `HollowNotch` | `Nebula6/Source/Dsp/HollowNotch.h` | SVF notch, parallel mix |
| `NebulaBus` | `Nebula6/Source/Dsp/NebulaBus.h` | Formant + air + ghost; **no alloc**; vowel morph table static |
| `TapeGravity` | `Nebula6/Source/Dsp/TapeGravity.h` | LFOs pre-computed block edge |
| `ShimmerHalo` | `Nebula6/Source/Dsp/ShimmerHalo.h` | 2-tap pitch-shift delay, feedback < 0.75 |
| `NebulaVoice` | `Nebula6/Source/Dsp/NebulaVoice.h` | `juce::SynthesiserVoice` |
| `NebulaEngine` | `Nebula6/Source/Dsp/NebulaEngine.h` | Voices + bus + FX; called from processor |
| `PerformanceEngine` | `templates/juce8-synth-base/Source/PerformanceEngine.*` | Reuse; dotted arp mode |

**RT rules:** chorus/delay lines sized in `prepareToPlay`; `frozen_latch` is atomic bool read once per block; MIDI export **never** on audio thread.

## Surface chain (control plane)

| Surface | Responsibility |
|---------|----------------|
| APVTS | All IDs in parameter-spec; macro router maps `macro1`…`macro8` → remarkable + DCO targets (control rate) |
| `Nebula6AudioProcessor` | Wires engine, state XML, program change loads preset row + tag |
| `PluginEditor` + WebView | Play shell contract: `Assets/` cyan/violet theme; chorus strip bound to `chorus_mode` + `frozen_latch` |
| `MacroRouter` | Nebula-specific curves (Sky → chorus+shimmer, etc.) |
| `Dice` | Message thread: randomize within `preset_tag` bounds only |
| `RenderSmokeTest` | Adapt from Play template; 3 tag presets |

### XY → program mapping

| Control | Program targets |
|---------|-----------------|
| `xy_x` | `filter_cutoff`, `hollow_notch` (darkness) |
| `xy_y` | `chorus_mix`, `shimmer_mix`, `nebula_level` (sky) |
| `layer_blend` / `plane_blend` | Equal-power core vs Nebula bus |

## Dual-product roadmap

| Product | Path | Scope |
|---------|------|-------|
| **Nebula6** desktop | `Nebula6/` @ repo root | Full chain + WebView + performance |
| **MpcJuno106** | `vst/MpcJuno106/` | Juno core only (headless) — **implement first or parallel extract** |
| **MpcNebula6** (v2) | `vst/MpcNebula6/` | Core + chorus; Nebula bus optional compile flag; K1–K3 |

## Build & verify

```bash
# After /apc-impl — scaffold from templates/juce8-synth-base, rename to Nebula6
./tools/build-vst.sh Nebula6   # when under vst/ OR custom cmake at repo root
Nebula6 --render-test          # standalone flag from Play pattern
```

**CI:** `sonic-proof.md` hero + tag renders; golden WAV when DSP stable.

## Extension points

- Unit tests on `shared/dsp/juno/JunoLpf24` stepped sine sweep
- Factory queue: `disklordz/vst-factory/data/nebula6_master.json` (design phase optional)