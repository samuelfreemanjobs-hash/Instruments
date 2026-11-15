# PlaySynthDreams — parameters

### Macros (host-visible, map to K1–K8 on optional MPC port)

| ID | UI label | Range |
|----|----------|-------|
| `macro_blend` | A / B Blend | 0..1 |
| `macro_tone` | Tone | 0..1 |
| `macro_drift` | Drift | 0..1 |
| `macro_lofi` | Lo-Fi | 0..1 |
| `macro_dirt` | Dirt | 0..1 |
| `macro_swell` | Swell | 0..1 |
| `macro_motion` | Motion | 0..1 |
| `macro_space` | Space | 0..1 |

### Layer A

| ID | Notes |
|----|-------|
| `layerA_wave1`, `layerA_wave2` | saw / tri / pulse |
| `layerA_detune`, `layerA_pw` | |
| `layerA_sub`, `layerA_fm_tine` | levels |
| `layerA_oct`, `layerA_pan` | |

### Layer B

| ID | Notes |
|----|-------|
| `layerB_wt_preset` | enum: bell / choir / flute / pad / pluck |
| `layerB_wt_pos` | morph 0..1 |
| `layerB_air` | vinyl/breath noise |
| `layerB_oct`, `layerB_pan` | |

### Filter & env

`cutoff`, `res`, `filt_env_a/d/s/r`, `filt_env_amt`, `amp_a/d/s/r`

### Master FX (also driven by macros)

`chorus_mix`, `delay_mix`, `delay_time_sync`, `reverb_mix`, `reverb_shimmer`, `master_level`

### Expert (optional collapse in UI)

Individual depths for macro mappings (defaults baked; advanced tab v1.1).