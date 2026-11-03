# DSP architecture specification

Version: 1.0  
Scope: VST/VSTi (JUCE) and logue SDK hardware units in this monorepo  
Normative for: `vst/`, `shared/dsp/`, `src/oscillators/`, `src/effects/`

---

## 1. Purpose

This document defines how audio plugins and hardware user units are structured so that:

- DSP, UI, and build targets stay separable.
- Parameters have one logical definition and many **surfaces** (DAW automation, hardware knobs, presets).
- Agents and humans can **assess complexity** before committing to a design.

Companion agent: **Audio Plugin Coder** (`.cursor/skills/audio-plugin-coder/`, rule `08-audio-plugin-coder.mdc`).

---

## 2. Architectural overview

```
┌─────────────────────────────────────────────────────────────────┐
│                        GLOBAL LAYER                              │
│  sample rate, block size, tuning, master bypass, RNG seed,       │
│  program index, cross-target manifest (params.json / manifest)   │
└────────────────────────────┬────────────────────────────────────┘
                             │
         ┌───────────────────┴───────────────────┐
         ▼                                       ▼
┌─────────────────────┐               ┌─────────────────────┐
│   SURFACE CHAIN       │               │   PROGRAM CHAIN      │
│   (control plane)     │   parameters  │   (audio plane)      │
│   UI, MIDI, host      │ ────────────► │   voices, FX, mix    │
│   automation, presets │   (mapped)    │   processBlock/CYCLE │
└─────────────────────┘               └─────────────────────┘
```

- **Program chain** — everything that runs on the **audio thread** at block rate or sample rate.
- **Surface chain** — everything that **reads/writes parameters** and **never blocks** the audio thread.
- **Global** — context shared by both chains, set at init/prepare and read-only on the audio thread (except atomics written from control thread).

---

## 3. Core components

### 3.1 Component catalog

| Component | Program chain | Surface chain | Global | Description |
|-----------|---------------|---------------|--------|-------------|
| **Context** | read | read | owner | `sampleRate`, `maxBlockSize`, channel layout |
| **ParameterBank** | read (smoothed) | write | schema | IDs, ranges, defaults, smoothing times |
| **VoiceManager** | yes | note events | — | MIDI → voice alloc, steal, legato |
| **Oscillator** | yes | — | — | Phase, wavetable, FM operators |
| **Filter** | yes | — | — | LP/HP/BP, key track |
| **Envelope** | yes | — | — | ADSR, MSEG |
| **LFO** | yes | rate sync | tempo | Mod sources |
| **ModMatrix** | yes | UI edit | — | Source → dest, depth |
| **FxChain** | yes | bypass | — | Serial/parallel FX slots |
| **Meter/Telemetry** | write atomics | read | — | Peak/RMS for UI only |
| **PresetStore** | apply snapshot | load/save | program # | Not on audio thread |
| **Editor** | — | yes | — | JUCE components, hardware labels |

### 3.2 Ownership rules

| Layer | Allowed on audio thread | Forbidden |
|-------|-------------------------|-----------|
| Program | Fixed storage, atomics read, preallocated buffers | `malloc`, locks, I/O, logging |
| Surface | APVTS writes, MIDI UI, file dialogs | DSP loops, large allocations in paint |
| Global | Const after `prepare` / `OSC_INIT` | Changing sample rate without re-prepare |

### 3.3 Directory mapping (repo)

| Spec component | VST (JUCE) | logue v1.1 | logue mkII |
|----------------|------------|------------|------------|
| Program | `Source/Dsp/*`, `processBlock` | `*.cc` `OSC_CYCLE` | `osc.h` `process()` |
| Surface | `PluginEditor`, APVTS | `OSC_PARAM`, `manifest.json` | `unit_set_param_value` |
| Global | `prepareToPlay`, `params.json` | `OSC_INIT`, `manifest.json` | `unit_init`, `header.c` |
| Schema | `vst/<Plugin>/params.json` | `manifest.json` `_agent` | `header.c` params |

### 3.4 Shared portable DSP (target)

Place **float, platform-agnostic** algorithms in `shared/dsp/`:

```
shared/dsp/
  envelope_adsr.h
  biquad.h
  phasor.h
  parameter_smoothing.h
```

- VST includes headers directly.
- logue wraps with q31 conversion at the boundary.

---

## 4. Program chain

The **program chain** is the ordered graph of audio processing executed per callback.

### 4.1 Callback entry points

| Target | Entry | Buffer contract |
|--------|-------|-----------------|
| VSTi/VST | `processBlock(buffer, midi)` | `numSamples` varies; use `getSampleRate()` |
| logue osc | `OSC_CYCLE` / `Osc::process` | `frames` ≤ 64 (v1); float (mkII) |
| logue FX | `*_PROCESS` | Main/sub buses per module |

### 4.2 Canonical synth program order

1. **Clear** output buffer (or accumulate for multi-bus).
2. **Ingest MIDI** (surface → program queue): note on/off, pitch bend, pressure.
3. **Voice tick** (per voice, per sample or per block):
   - Envelopes → oscillators → filter → voice gain.
4. **Bus sum** → voice mixer.
5. **FxChain** (insert / send / master).
6. **Output trim** + soft clip (optional).
7. **Meters** (atomic peaks).

### 4.3 Canonical effect program order

1. **Input** (dry copy for parallel path).
2. **Input gain** + HPF (optional).
3. **Core algorithm** (delay, reverb, saturation).
4. **Mix** (dry/wet).
5. **Output gain**.

### 4.4 Block vs sample processing

| Strategy | When to use | Complexity |
|----------|-------------|------------|
| Per-sample | Few voices, simple FM | Low–medium |
| Per-block coeffs | Filters with stable params | Medium |
| Oversampling inner loop | Nonlinear / waveshaping | High |

Program chain code must **not** depend on Editor classes or JUCE GUI modules.

---

## 5. Surface chain

The **surface chain** maps human and host intent to **ParameterBank** values.

### 5.1 Surfaces (inputs)

| Surface | VST | logue |
|---------|-----|-------|
| Host automation | APVTS / normalized 0–1 | N/A |
| Plugin UI | Sliders, attachments | N/A |
| Hardware knobs | N/A | `OSC_PARAM` / `unit_set_param_value` |
| MIDI CC | `MidiMapping` (optional) | N/A |
| Presets | XML state / file | `manifest.json` `_agent.presets` |

### 5.2 Surface responsibilities

1. Display current parameter values (from APVTS or cached IDs).
2. Write user gestures → **raw** parameter values (host handles automation recording).
3. Load/save presets **off** the audio thread.
4. Map labels, units, and enum strings (mkII `getParameterStrValue`).

### 5.3 Forbidden in surface chain

- Calling `processBlock` or reading audio buffers for DSP.
- Long synchronous file I/O on message thread without async (use `MessageManager::callAsync`).

---

## 6. Global layer

### 6.1 Global context struct (conceptual)

```cpp
struct GlobalContext {
  double sampleRate;
  int maxBlockSize;
  int numChannels;
  float tuningCents;       // optional
  uint32_t programIndex;
  uint64_t transportFrame; // optional, from host
};
```

### 6.2 Global lifecycle

| Phase | VST | logue |
|-------|-----|-------|
| Construct | C++ ctor, default params | `OSC_INIT` / `unit_init` |
| Activate | `prepareToPlay` | first `OSC_CYCLE` / `unit_render` |
| Run | read-only `GlobalContext` on audio thread | same |
| Deactivate | `releaseResources` | `unit_teardown` |

### 6.3 Cross-target manifest

**VST:** `vst/<Plugin>/params.json` (recommended)  
**logue:** `manifest.json` `header.params` + `_agent.presets`

Both should list the same **logical parameter IDs** where one product ships on multiple targets.

Example `params.json` shape:

```json
{
  "version": 1,
  "parameters": [
    {
      "id": "gain",
      "name": "Gain",
      "type": "float",
      "min": 0,
      "max": 1,
      "default": 0.7,
      "smoothingMs": 10,
      "targets": { "vst": "gain", "logue": 0 }
    }
  ],
  "presets": [
    { "name": "Init", "values": { "gain": 0.7 } }
  ]
}
```

---

## 7. Parameter mapping

### 7.1 Layers of mapping

```
Logical ID (params.json)
    → Surface value (UI raw / MIDI CC)
    → Normalized value (0..1 or enum index)
    → DSP value (Hz, seconds, linear gain)
    → Smoothed DSP value (audio thread)
```

### 7.2 Mapping functions

| Type | UI range | DSP conversion |
|------|----------|----------------|
| Linear gain | 0..1 | direct |
| Frequency | 0..1 | `exp(log(fMin) + norm * (log(fMax)-log(fMin)))` |
| Time | 0..1 | squared or exponential for ADSR |
| Enum | 0..N-1 | integer slot |
| logue 0..100 | knob display | `norm = value / 100.f` |
| mkII 0..1023 | hardware | `param_10bit_to_f32` |

### 7.3 Smoothing

Every **DSP-critical** parameter should declare `smoothingMs` in schema.

- Control thread: target from APVTS.
- Audio thread: one-pole or ramp toward target; never step discontinuities on filter cutoff or gain.

### 7.4 Modulation (optional)

ModMatrix entries:

```text
source ∈ { LFO1, LFO2, Env2, Velocity, Pressure }
dest   ∈ { parameter logical ID }
depth  ∈ [-1, 1]
```

Applied in program chain **after** base smoothed value.

---

## 8. Complexity assessment

Use before implementation to pick architecture and estimate risk.

### 8.1 Scoring dimensions (0–3 each)

| Dimension | 0 | 1 | 2 | 3 |
|-----------|---|---|---|---|
| **Voices** | 0 (FX only) | 1 mono | 2–8 poly | 16+ / unison |
| **Oscillators per voice** | 0 | 1 | 2–3 | FM/wavetable stack |
| **Filters** | 0 | 1 static | 2+ / nonlinear | ZDF / oversampled |
| **Modulation** | 0 | 1 LFO | Matrix | MSEG + sync |
| **FX** | 0 | 1 insert | Chain | IR / long delay |
| **Surfaces** | VST only | + presets | + MIDI map | + HW port |
| **Targets** | 1 | VST + 1 HW | mkI + mkII | XD + Prologue + VST |

**Total score** = sum of dimensions (max 21).

### 8.2 Complexity tiers

| Tier | Score | Architecture guidance |
|------|-------|------------------------|
| **L0 Minimal** | 0–4 | Single file Processor; inline DSP |
| **L1 Standard** | 5–8 | `Source/Dsp/` + VoiceManager; params.json |
| **L2 Advanced** | 9–13 | ModMatrix, FxChain, separate unit tests |
| **L3 Expert** | 14–21 | Oversampling, SIMD, dual-target shared/dsp |

### 8.3 Real-time risk flags (any tier)

- Dynamic voice allocation per note → **reject**; use fixed pool.
- File I/O in `processBlock` / `OSC_CYCLE` → **reject**.
- Unbounded recursion or `printf` in audio → **reject**.
- Double precision on Cortex-M4 logue → **reject**.

### 8.4 Agent workflow

1. Run skill **dsp-complexity-assessment** → record tier + flags.
2. If L2+, require **params.json** and `shared/dsp/` modules.
3. Subagent **dsp-program-chain** implements program graph.
4. Subagent **dsp-surface-chain** implements APVTS + editor.
5. Subagent **plugin-build-verify** compiles VST; **logue-hardware-port** if scoped.

---

## 9. Compliance checklist

Before marking a plugin “architecture complete”:

- [ ] Logical parameters documented in `params.json` or `manifest.json`
- [ ] Program chain diagram in plugin `README.md` (one paragraph or ASCII)
- [ ] No GUI headers included from `Source/Dsp/`
- [ ] Smoothing on frequency/gain params
- [ ] Complexity tier recorded in `README.md` or PR description
- [ ] VST created via `./tools/new-vst.sh` from template
- [ ] logue units remain under `src/oscillators/` with separate port

---

## 10. References

- [dsp-architecture-specification.md](dsp-architecture-specification.md) (this file)
- [vst/README.md](../vst/README.md)
- [nts1-mkii-porting.md](nts1-mkii-porting.md)
- `.cursor/rules/06-vst-vsti-template.mdc`
- `.cursor/rules/08-audio-plugin-coder.mdc`
