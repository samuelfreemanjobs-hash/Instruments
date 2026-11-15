# MpcJuno106 — parameters (v1)

Melodee **K1–K3** = IDs 1–3.

| # | ID | Name | Range | Default |
|---|-----|------|-------|---------|
| 1 | `cutoff` | Filter cutoff | 20 Hz–12 kHz (skew) | 2000 |
| 2 | `res` | Resonance | 0–1 | 0.15 |
| 3 | `chorus` | Chorus mix | 0–1 | 0.35 |
| 4 | `dco_saw` | Saw level | 0–1 | 0.8 |
| 5 | `dco_pulse` | Pulse level | 0–1 | 0 |
| 6 | `pulse_width` | Pulse width | 0.1–0.9 | 0.5 |
| 7 | `sub_level` | Sub osc | 0–1 | 0.3 |
| 8 | `attack` | Amp attack | 1 ms–3 s | 5 ms |
| 9 | `decay` | Amp decay | 1 ms–5 s | 400 ms |
| 10 | `sustain` | Amp sustain | 0–1 | 0.85 |
| 11 | `release` | Amp release | 1 ms–8 s | 300 ms |
| 12 | `lfo_rate` | LFO rate | 0.05–20 Hz | 3 |
| 13 | `lfo_to_pwm` | LFO → PWM | 0–1 | 0 |

**Programs (ship):** Init, Chorus Pad, Funk Bass, Brass Stab (factory table in impl).