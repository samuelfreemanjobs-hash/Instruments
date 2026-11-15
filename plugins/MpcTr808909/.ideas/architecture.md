# MpcTr808909 — architecture (APC Plan)

## Core components

| Voice | Model |
|-------|--------|
| Kick | Sine osc + pitch EG (exp decay on frequency); optional click (short noise burst) for 909 |
| Snare | Sine/tone BP + white noise; independent decays, summed |
| Hat (CH/OH) | Square (metal) + HPF noise; OH longer decay via note or vel |
| Cowbell | Two square oscs (1.0 + 1.5 ratio typical) |
| Rim | Very short sine click |
| Tom | Pitched sine sweep (shared Kick engine, different defaults per note) |

| Engine | Role |
|--------|------|
| `DrumVoiceBase` | `trigger(note, vel)`, `render()`, `isActive()` |
| `DrumMixer` | Sum active voices; soft clip `drive` |
| `MidiDrumMap` | Note → voice factory |
| `FocusMapper` | Maps K1–K3 to `focus_voice` params for Melodee |

## Program chain

```text
MIDI drum note → spawn/steal voice slot for that instrument type
per voice per block:
  render synth into temp buffer (mono)
DrumMixer: sum voices → tanh(drive) → stereo out
```

**Polyphony:** up to **8 simultaneous** hits (one slot per class, or pool of 12 one-shots — Design chooses steal policy).

**RT:** voices are structs in `std::array`; no alloc on trigger.

## Surface

Headless; **kit** + **focus_voice** change which K1–K3 affect; programs store full per-voice tables.

## Targets

VST3 x86_64 + aarch64; intended as **MPC drum track** instrument (midi channel 10).