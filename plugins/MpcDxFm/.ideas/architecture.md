# MpcDxFm — architecture (APC Plan)

**Normative:** [docs/dsp-architecture-specification.md](../../../docs/dsp-architecture-specification.md)

## Core components

| Component | Location | Role |
|-----------|----------|------|
| `FmVoice` | `Source/Dsp/FmVoice.h` | Fixed pool; note on/off; per-voice op phases |
| `FmOperator` | `Source/Dsp/FmOperator.h` | sine (± lightweight multisi wave); phase accum; feedback buffer |
| `AlgorithmGraph` | `Source/Dsp/FmAlgorithms4.h` | 8 topologies (serial/parallel stacks) |
| `VoiceManager` | processor | 16 voices (4-op) or 8 (6-op) compile/runtime flag |
| Output scale | inline | tanh or soft clip optional on sum |

## Program chain

```text
MIDI → allocate voice from fixed pool
per voice per block:
  for each sample (or 2–4 sample inner loop):
    run AlgorithmGraph: op4 → op3 → op2 → op1 (per algo routing)
    each op: phase += freq(ratio * note + detune + lfo)
              output = sin(phase + feedback * last_op1)
    × global ADSR → accumulator
sum all active voices → stereo (mono duplicate; optional pan spread v2)
```

**No VCF v1** — brightness via operator levels + feedback (authentic DX).

## Operator math

- Phase in float; `sin` via `std::sin` or small wavetable (256) for speed on ARM.
- **Feedback** only on operator 1 (classic DX7 style) in v1.

## RT rules

- Fixed `std::array<FmVoice, kMaxVoices>` — no alloc.
- Algorithm index changes only on control thread / block edge (smooth crossfade optional in Design).

## Surface

Headless; programs bundle ratio/level sets for EP, bell, bass, lead.

## Targets

VST3 x86_64 + aarch64; poly count validated on MPC Sample before shipping 16-voice default.