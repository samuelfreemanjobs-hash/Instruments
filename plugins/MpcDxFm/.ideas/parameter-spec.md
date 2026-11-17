# MpcDxFm — parameters (v1)

Melodee **K1–K3** on performance macros (map to underlying ops in impl).

| # | ID | Name | Range | Default |
|---|-----|------|-------|---------|
| 1 | `brightness` | Brightness (op2 level) | 0–1 | 0.55 |
| 2 | `feedback` | Op1 feedback | 0–1 | 0.12 |
| 3 | `algo` | Algorithm | 0–7 | 3 |
| 4 | `op1_level` | Op1 level | 0–1 | 1 |
| 5 | `op2_level` | Op2 level | 0–1 | 0.7 |
| 6 | `op3_level` | Op3 level | 0–1 | 0.5 |
| 7 | `op4_level` | Op4 level | 0–1 | 0.6 |
| 8 | `op1_ratio` | Op1 ratio | 0.5–16 | 1 |
| 9 | `op2_ratio` | Op2 ratio | 0.5–16 | 2 |
| 10 | `op3_ratio` | Op3 ratio | 0.5–16 | 1 |
| 11 | `op4_ratio` | Op4 ratio | 0.5–16 | 3 |
| 12 | `op1_detune` | Op1 detune cents | −50–50 | 0 |
| 13 | `op2_detune` | Op2 detune | cents | 0 |
| 14 | `attack` | Global amp attack | 0–3 s | 0.01 |
| 15 | `decay` | Decay | 0–3 s | 0.4 |
| 16 | `sustain` | Sustain | 0–1 | 0.75 |
| 17 | `release` | Release | 0–4 s | 0.3 |
| 18 | `lfo_rate` | LFO | 0.1–12 Hz | 0 |
| 19 | `lfo_depth` | LFO → pitch | 0–1 | 0 |
| 20 | `voice_mode` | 4-op / 6-op | 0=4 1=6 | 0 |

**Programs:** EPiano, Bell, Slap Bass, G Lead, FM Pad, Init.

**Per-op envelopes (v2):** single global ADSR in v1 for CPU; design may add 2-rate (attack/decay per modulator) if profiling allows.