# MpcTr808909 — parameters (v1)

### Global (Melodee K1–K3 when focus = Kick)

| # | ID | Name | Range | Default |
|---|-----|------|-------|---------|
| 1 | `focus_decay` | Decay (focused) | 10 ms–3 s | 400 ms |
| 2 | `focus_tune` | Tune / sweep | 20–200 Hz kick; sem for others | 55 |
| 3 | `drive` | Bus drive | 0–1 | 0.2 |
| 4 | `focus_voice` | Edit target | 0=kick 1=snare 2=hat 3=cowbell | 0 |
| 5 | `kit` | Kit | 0=808 1=909 2=hybrid | 0 |

### Per-voice (APVTS; loaded by focus or full state)

| ID | Kick | Snare | Hat | Cowbell |
|----|------|-------|-----|---------|
| `*_decay` | amp env | tone+noise | decay | decay |
| `*_pitch` | sweep start→end | tone Hz | metal tune | ratio |
| `*_tone` | sine | bandpass tone | square mix | dual sq |
| `*_noise` | — | snare noise | hat noise | — |
| `*_click` | 909 click level | — | — | — |
| `*_sweep` | sweep time ms | — | — | — |

**Kick sweep (808):** `kick_pitch_start`, `kick_pitch_end`, `kick_sweep_ms` — exponential pitch drop on sine.

**Snare:** `snare_tone_freq`, `snare_noise_level`, `snare_tone_decay`, `snare_noise_decay` (bridged-T style overlap).

**Hat:** `hat_square_level`, `hat_noise_level`, `hat_decay` (short), optional HPF.

**Programs:** 808 Classic, 909 House, Hybrid Punch, Kick Sub, Rim Lite.