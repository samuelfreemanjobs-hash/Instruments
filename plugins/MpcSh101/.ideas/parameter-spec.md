# MpcSh101 — parameters (v1)

Melodee **K1–K3** = IDs 1–3.

| # | ID | Name | Range | Default |
|---|-----|------|-------|---------|
| 1 | `cutoff` | VCF cutoff | 30 Hz–16 kHz | 800 |
| 2 | `res` | Resonance | 0–1 | 0.35 |
| 3 | `sub_level` | Sub osc | 0–1 | 0.5 |
| 4 | `vco_wave` | VCO | 0=saw 1=pulse | 0 |
| 5 | `pulse_width` | PWM / width | 0.1–0.9 | 0.5 |
| 6 | `vco_level` | VCO level | 0–1 | 1.0 |
| 7 | `glide` | Portamento | 0–2 s | 0 |
| 8 | `pw_lfo_rate` | PWM LFO rate | 0–15 Hz | 0 |
| 9 | `pw_lfo_depth` | PWM LFO depth | 0–1 | 0 |
| 10 | `vca_attack` | VCA attack | 0.5 ms–2 s | 2 ms |
| 11 | `vca_decay` | VCA decay | 1 ms–4 s | 180 ms |
| 12 | `vca_sustain` | VCA sustain | 0–1 | 0 |
| 13 | `vca_release` | VCA release | 1 ms–4 s | 80 ms |
| 14 | `vcf_eg_amt` | VCF env amount | 0–1 | 0.6 |
| 15 | `vcf_attack` | VCF attack | 0.5 ms–2 s | 2 ms |
| 16 | `vcf_decay` | VCF decay | 1 ms–4 s | 200 ms |
| 17 | `vcf_sustain` | VCF sustain | 0–1 | 0 |
| 18 | `vcf_release` | VCF release | 1 ms–4 s | 100 ms |
| 19 | `env_mode` | Env mode | 0=AD 1=ADS | 0 |

**Programs:** Init Bass, Acid Punch, Sub Weight, Short Lead, PWM Sweep.

**Note:** Default **VCA sustain 0** matches snappy 101 bass; lead programs raise sustain.