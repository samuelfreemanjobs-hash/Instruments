# OxfordPeak8 — parameters (v1)

### Oscillators 1–3 (shared APVTS, per-voice DSP)

| ID pattern | Description |
|------------|-------------|
| `oscN_wave` | sine / tri / saw / pulse / wavetable |
| `oscN_shape` | PWM or wavetable position twist 0..1 |
| `oscN_oct`, `oscN_fine` | octave, fine cents |
| `oscN_level` | mixer 0..1 |
| `fm_12`, `fm_23`, `fm_31` | linear FM amounts 0..1 |
| `noise_level` | white 0..1 |
| `noise_pink` | 0..1 optional v1.1 |

### Filter

| ID | Notes |
|----|-------|
| `filter_mode` | LP / BP / HP |
| `filter_slope` | 12dB / 24dB |
| `cutoff`, `resonance` | |
| `filter_keytrack` | |
| `filter_divergence` | per-voice cutoff spread 0..1 |

### Drive

| ID | Stage |
|----|-------|
| `drive_pre` | pre-filter |
| `drive_post` | post-filter |
| `drive_master` | global |

### Envelopes

`amp_a/d/s/r`, `modenv1_*`, `modenv2_*`, `modenvN_loop` (bool)

### LFOs 1–4

`lfoN_wave`, `lfoN_rate`, `lfoN_sync`, `lfoN_fade`, `lfoN_poly` (1–2 poly, 3–4 global)

### Mod matrix (16 slots)

`modN_srcA`, `modN_srcB`, `modN_dest`, `modN_depth`

**ModSource enum:** None, LFO1–4, ModEnv1–2, Velocity, ModWheel, Aftertouch, KeyTrack, AmpEnv, Direct (1.0).

**ModDest enum:** Osc1–3 Pitch, Osc1–3 Shape, FM12/23/31, FilterCutoff, FilterRes, DrivePre/Post/Master, LFO1–4 Rate, AmpLevel, Pan.

### FX

`chorus_mode`, `chorus_mix`, `delay_mix`, `delay_time`, `delay_damp`, `reverb_mix`, `reverb_predelay`, `reverb_damp`, `master_level`