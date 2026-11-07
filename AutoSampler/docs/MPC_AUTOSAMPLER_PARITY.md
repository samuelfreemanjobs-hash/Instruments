# MPC Software Auto Sampler — control parity

Auto Sampler mirrors the MPC Auto Sampler timing model (see also community tooling such as [Mozaic autosampler](https://github.com/os68ng/mozaic-patches/blob/main/tools/autosampler.moz)).

| MPC concept | Auto Sampler parameter | Notes |
|-------------|------------------------|--------|
| Start / End note | Low Note / High Note | MIDI 0–127 |
| Interval (semitones) | Note Step + Note Map | Map presets: chromatic, 3rd, 5th, octave |
| Gate (note hold) | **Gate (ms)** | MIDI note on duration before note-off |
| Release (tail) | **Release (ms)** | Record time after note-off |
| Pause between samples | **Pause (ms)** | Former “gap” |
| Preroll | **Preroll (ms)** | Capture audio before note-on |
| Velocity layers V1–V4 | **Velocity 1–4** | `0` = off (MPC-style) |
| MIDI channel | **MIDI Channel** | 1–16 |
| Session estimate | UI status line | Keys × vels × (gate+release+pause) |

## Overnight JSON (`capture`)

```json
"noteHoldMs": 1000,
"tailMs": 2500,
"prerollMs": 0,
"pauseMs": 500,
"velocity1": 127,
"velocity2": 96,
"velocity3": 64,
"velocity4": 32,
"midiChannel": 1
```

Legacy `recordSeconds` in JSON still works: split **35% gate / 65% release** when `noteHoldMs` is omitted.
