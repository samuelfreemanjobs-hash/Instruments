# MpcLaSynth — architecture (APC Plan)

## Concept (D-50 / M1 style)

```text
Output = Attack_PCM(t) × envA(t) + Body_Loop(phase) × envB(t)
         → SVF LP (cutoff, res) → optional light chorus → voice out
```

## Core components

| Component | Role |
|-----------|------|
| `EmbeddedWaveBank` | `Resources/attacks/*.h` + `Resources/bodies/*.h` — mono 44.1k float or int16 |
| `LaVoice` | Playback pointer for attack; loop phase for body; per-partial gains |
| `EnvAttack` | One-shot + optional crossfade at handoff to body |
| `EnvBody` | ADS on loop (release on note off) |
| `Tvf` | `StateVariableTPTFilter` — shared per voice post-sum |
| `ChorusLite` | Optional stereo mod delay (reuse Juno chorus module pattern) |
| `VoicePool` | Fixed 8 (MPC) / 16 (desktop) |

## Program chain

```text
MIDI on → grab free LaVoice
  attack: read embedded PCM[idx] at rate = note pitch factor × root
  body: wavetable step = f(note, body_wave), loop wrap
  mix × levels → TVF → chorus → pan → bus
MIDI off → release EG; free voice when silent
```

**Pitch:** attack samples **repitch** via linear interpolation resampling (short tables only).

**RT:** all PCM pointers const; no file I/O; voice struct preallocated.

## Content pipeline (Design → Impl)

1. Author or synthesize 6 attacks + 6 loops offline (not in audio thread).
2. Convert with `tools/embed-pcm.py` (to be added in Impl) → headers.
3. License: **royalty-free generated** or original; **no** M1/ D-50 ROM dumps.

## Surface

Headless; program = attack_idx + body_idx + EG + filter presets.

## Targets

VST3 x86_64 + aarch64; binary size budget documented in Design (<250 KB PCM).