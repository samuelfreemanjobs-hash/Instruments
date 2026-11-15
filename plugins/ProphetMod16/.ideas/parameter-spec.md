# ProphetMod16 — parameter spec (v1)

### Oscillators (per voice, global APVTS)

| ID | Name | Range | Notes |
|----|------|-------|-------|
| `osc1_wave` | Osc 1 wave | enum saw/tri/sawtri/pulse | |
| `osc2_wave` | Osc 2 wave | same | |
| `osc1_oct` | Osc 1 octave | -2..+2 | |
| `osc2_oct` | Osc 2 octave | -2..+2 | |
| `osc1_fine` | Osc 1 fine | -50..+50 ct | |
| `osc2_fine` | Osc 2 fine | -50..+50 ct | |
| `osc1_pw` | Osc 1 PW | 0..1 | |
| `osc2_pw` | Osc 2 PW | 0..1 | |
| `osc_mix` | Osc mix | 0..1 | 0=osc1, 1=osc2 |
| `sub_level` | Sub level | 0..1 | -1 oct square |
| `noise_level` | Noise | 0..1 | white |
| `voice_slop` | Voice slop | 0..1 | pitch ±12 ct, cutoff ±40 Hz drift |

### Filter

| ID | Name | Range |
|----|------|-------|
| `cutoff` | Cutoff | 20 Hz–20 kHz |
| `resonance` | Resonance | 0..1 |
| `filter_poles` | Poles | 0=12dB, 1=24dB |
| `filter_drive` | Pre drive | 0..1 |
| `filter_keytrack` | Key track | 0..1 |

### Envelopes

| ID | Name | Notes |
|----|------|-------|
| `amp_a/d/s/r` | Amp ADSR | velocity scaled |
| `filt_env_a/d/s/r` | Filter ADSR | |
| `filt_env_amt` | Filter env amt | -1..+1 bipolar → cutoff |
| `aux_a/d/s/r` | Aux ADSR | matrix source |
| `aux_env_amt` | Aux default amt | optional bias |

### LFOs 1–4

| ID | Pattern |
|----|---------|
| `lfoN_wave` | tri/saw/ramp/square/random |
| `lfoN_rate` | Hz or sync division |
| `lfoN_sync` | bool |
| `lfoN_depth` | 0..1 global (per-patch) |

### Mod matrix (8 slots)

| ID | Range |
|----|-------|
| `modN_src` | ModSource enum |
| `modN_dst` | ModDest enum |
| `modN_amt` | -1..+1 |

### Master

| ID | Name |
|----|------|
| `voice_mode` | poly / mono legato / unison (4 detune) |
| `unison_detune` | cents spread |
| `chorus_mix` | 0..1 |
| `delay_mix` | 0..1 |
| `delay_time` | sync or ms |
| `master_level` | 0..1 |