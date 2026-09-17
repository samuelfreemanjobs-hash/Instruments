# TR-808 recreation — architecture blueprint

**Shipping name:** TBD — [docs/PRODUCT_NAMING.md](docs/PRODUCT_NAMING.md)  
**Stack:** **JUCE 8** + VST3 + CLAP (Disklordz factory). **Windows-first.**  
**Deep spec:** [docs/circuit-emulation-spec.md](docs/circuit-emulation-spec.md)

## Design intent

- **Sixteen voices**, **discrete analog voice circuits** emulated in software — **not** sample playback.
- MVP build order: **BD → SD → CH/OH → CP**, then metals, toms/congas, cymbal.
- **MIDI** in; **kit previous/next**; **CH/OH choke**; onboard sequencer deferred.

## Four architectural classes (hardware)

| Class | Voices (examples) | Modeling notes |
|-------|-------------------|----------------|
| Bridged-T resonators | BD, toms, congas, claves, rim | Op-amp loop gain = decay; WDF / MNA R-type adaptors |
| Schmitt hex cluster | Cowbell, cymbal, OH/CH metals | Free-running oscillators; PolyBLEP; VCA gating only |
| Noise + shapers | SD snappy, clap, maracas | BJT breakdown noise; multi-pulse clap; BPF formants |
| Hybrid | SD tonal + noise, BD pulse shaper + retrigger | ODE / Newton or Lambert-W LUT for diodes |

## Runtime layers (target)

```text
Host (VST3/CLAP) → MIDI/trigger + accent → lock-free sequencer (later)
    → 16 voice DSP (SIMD where applicable) → per-voice saturation (later)
    → master bus / multi-out (MVP: stereo) → DAW
UI (GPU) ↔ parameter FIFOs (no alloc on audio thread)
```

## Plugin shell (MVP)

- JUCE processor + minimal UI: kit name, prev/next, per-voice level trims.
- Preset = kit parameter snapshot + kit index.
- `prepareToPlay`: all state/buffers allocated; voices sleep below −120 dBFS with **state retained**.

## QA (when code exists)

- Host smoke: Reaper + one other Windows DAW
- Choke: CH closes OH tail
- Retrigger: fast BD/CH hits show phase-dependent level (non–machine-gun)
- pluginval when CLAP/VST3 targets exist

## Repo layout (target)

```text
vst-tr808/
├── REPO_HANDOFF.md
├── docs/circuit-emulation-spec.md
├── Source/                 ← JUCE (future)
└── presets/
```

## Priority vs Junova-X

**Junova-X:** P0 — JUCE host green (WO-2026-001…).  
**TR-808:** P1 — spec + FAUST/WDF prototypes in parallel; **no** full implementation WO until Junova smoke or Planner override.
