# SemV — parameters (v1)

Match Arturia SEM V **musical** layout; IDs are stable for APVTS / automation.

## Global

| ID | Name | Range | Default | Notes |
|----|------|-------|---------|-------|
| `master` | Master | 0–1 | 0.75 | Output trim |
| `voices` | Voice limit | 4–16 | 8 | Fixed pool size at init |
| `poly_mode` | Poly / Mono | bool | poly | Mono = legato |
| `glide_ms` | Glide | 0–2000 ms | 0 | Mono legato only |
| `tuning` | Fine tune | −50–+50 ct | 0 | Global |

## Oscillators (per voice)

| ID | Name | Range | Default |
|----|------|-------|---------|
| `osc1_level` | Osc 1 | 0–1 | 0.8 |
| `osc1_wave` | Osc 1 wave | 0=saw, 1=pulse | saw |
| `osc1_pw` | Pulse width | 0.05–0.95 | 0.5 |
| `osc2_level` | Osc 2 | 0–1 | 0.6 |
| `osc2_wave` | Osc 2 wave | 0=saw, 1=pulse | pulse |
| `osc2_pw` | Pulse width | 0.05–0.95 | 0.45 |
| `osc2_detune` | Osc 2 detune | −12–+12 st | +7 ct |
| `sub_level` | Sub osc | 0–1 | 0.25 |
| `noise_level` | Noise | 0–1 | 0 |

## State-variable filter (SEM)

| ID | Name | Range | Default |
|----|------|-------|---------|
| `filter_cutoff` | Cutoff | 20 Hz–18 kHz | 2.5 kHz |
| `filter_reso` | Resonance | 0–1 | 0.35 |
| `filter_mode` | Mode | LP/BP/HP/Notch | LP |
| `filter_env_amt` | Env → filter | −1–+1 | 0.4 |
| `filter_kbd` | Key track | 0–1 | 0.5 |

## Envelopes

| ID | Name | Range | Default |
|----|------|-------|---------|
| `amp_attack` | Amp A | 1 ms–8 s | 5 ms |
| `amp_decay` | Amp D | 1 ms–8 s | 400 ms |
| `amp_sustain` | Amp S | 0–1 | 0.7 |
| `amp_release` | Amp R | 1 ms–12 s | 300 ms |
| `filt_attack` | Filt A | 1 ms–4 s | 10 ms |
| `filt_decay` | Filt D | 1 ms–4 s | 600 ms |
| `filt_sustain` | Filt S | 0–1 | 0.3 |
| `filt_release` | Filt R | 1 ms–8 s | 400 ms |

## LFO 1

| ID | Name | Range | Default |
|----|------|-------|---------|
| `lfo_rate` | Rate | 0.01–20 Hz | 2 Hz |
| `lfo_depth` | Depth | 0–1 | 0 |
| `lfo_dest` | Destination | Off/PW/Cutoff/Amp | Off |
| `lfo_sync` | Tempo sync | bool | off |

## Arturia-style insert FX (simplified)

| ID | Name | Range | Default |
|----|------|-------|---------|
| `chorus_mix` | Chorus | 0–1 | 0.15 |
| `delay_mix` | Delay | 0–1 | 0 |
| `delay_time` | Delay time | 1–750 ms | 320 ms |
| `reverb_mix` | Reverb | 0–1 | 0.2 |

## Performance / host

| ID | Name | Range | Default |
|----|------|-------|---------|
| `bend_range` | Pitch bend | ±2–24 st | ±2 |
| `vel_sens` | Velocity → amp | 0–1 | 0.85 |

**Presets (v1 factory, 12):** Init SEM, Brass Sweep, Notch Lead, PWM Strings, Mono Bass SEM, Reso Pluck, Soft Pad, Acid Notch, Chorus Keys, Delay Arp (host arp), Dry Lead, Wide Stack.