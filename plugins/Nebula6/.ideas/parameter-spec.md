# Nebula6 — parameters (Dream v1)

Stable APVTS IDs for automation, WebView, and future MpcNebula6 K-mapping.

## Global & performance

| ID | Name | Range | Default | Notes |
|----|------|-------|---------|-------|
| `master_level` | Master | 0–1 | 0.78 | After FX |
| `voice_limit` | Voices | 4–12 | 8 | Fixed pool at prepare |
| `poly_mode` | Poly | 0 poly / 1 mono | poly | |
| `glide_ms` | Glide | 0–2000 ms | 120 | Legato mono |
| `bend_range` | Bend | ±2–12 st | ±2 | |
| `scale_lock` | Scale | Off, Cm, F# Phryg, D Dorian | Cm | PerformanceEngine |
| `arp_enable` | Arp | bool | off | |
| `arp_mode` | Arp mode | Up/Down/UpDown/Random/Dotted | UpDown | Dotted = phonkwave |
| `arp_rate` | Arp rate | 1/8, 1/16 | 1/8 | Host sync |
| `arp_gate` | Gate | 0.1–1 | 0.65 | |
| `chord_mode` | Chord | Off/Oct/Min7/Min9/DarkTriad | Min7 | |
| `midi_export_trigger` | Export MIDI | momentary | — | UI thread → `~/Documents/NebulaExports/` |
| `macro_lock` | Macro lock | bool | off | Play contract |
| `dice_trigger` | Dice | momentary | — | Tag-bounded random |

## Plane A — 106 core (per voice)

| ID | Name | Range | Default |
|----|------|-------|---------|
| `dco_saw` | Saw level | 0–1 | 0.7 |
| `dco_pulse` | Pulse level | 0–1 | 0.5 |
| `dco_pwm` | PWM width | 0.05–0.95 | 0.45 |
| `dco_sub` | Sub level | 0–1 | 0.35 |
| `dco_drift` | Thermal drift | 0–1 | 0.15 | Control rate |
| `filter_cutoff` | Cutoff | 20 Hz–18 kHz | 3.2 kHz |
| `filter_reso` | Resonance | 0–1 | 0.28 |
| `filter_env_amt` | Filt env | −1–+1 | 0.35 |
| `amp_attack` | A | 1 ms–8 s | 8 ms |
| `amp_decay` | D | 1 ms–8 s | 350 ms |
| `amp_sustain` | S | 0–1 | 0.75 |
| `amp_release` | R | 1 ms–12 s | 800 ms |
| `filt_attack` | Filt A | 1 ms–4 s | 15 ms |
| `filt_decay` | Filt D | 1 ms–4 s | 500 ms |
| `filt_sustain` | Filt S | 0–1 | 0.25 |
| `filt_release` | Filt R | 1 ms–8 s | 600 ms |

## Remarkable controls (instrument identity)

| ID | Name | Range | Default | Behaviour |
|----|------|-------|---------|-----------|
| `chorus_mode` | Chorus | I / II / Drift / Frozen | II | Drift links wow; Frozen latches BBD phase |
| `chorus_mix` | Chorus depth | 0–1 | 0.55 | |
| `chorus_rate` | Chorus rate | 0.01–8 Hz | 0.6 | Mod depth in Drift |
| `frozen_latch` | Frozen | bool | off | Latches chorus LFO at current phase |
| `drift_couple` | Drift couple | 0–1 | 0.5 | Wow ↔ chorus rate amount |
| `hollow_notch` | Hollow | 0–1 | 0 | Parallel notch send post-LP |
| `nebula_level` | Nebula | 0–1 | 0.4 | Plane B formant + air |
| `nebula_vowel` | Vowel morph | 0–1 | 0.5 | Formant cloud |
| `ghost_overtone` | Ghost 808 | 0–1 | 0 | Bandpassed ratio ghost |
| `tape_gravity` | Tape | 0–1 | 0.2 | Wow/flutter + soft clip |
| `shimmer_mix` | Shimmer | 0–1 | 0.25 | Cloud halo feedback |
| `plane_blend` | Core vs haze | 0–1 | 0.65 | Equal-power DCO vs bus return |

## Play surface (XY + macros)

| ID | Name | Range | Default | Maps to |
|----|------|-------|---------|---------|
| `layer_blend` | A/B blend | 0–1 | 0.65 | `plane_blend` alias |
| `xy_x` | XY X | 0–1 | 0.5 | Darkness ← → brightness |
| `xy_y` | XY Y | 0–1 | 0.5 | Body ← → Sky |
| `macro1` | **Sky** | 0–1 | 0.5 | chorus + shimmer |
| `macro2` | **Drift** | 0–1 | 0.35 | tape + drift_couple |
| `macro3` | **Hollow** | 0–1 | 0.2 | hollow_notch + cutoff shade |
| `macro4` | **Nebula** | 0–1 | 0.45 | nebula_level + vowel |
| `macro5` | **Tone** | 0–1 | 0.5 | cutoff/res macro |
| `macro6` | **Body** | 0–1 | 0.5 | saw/pulse/sub mix |
| `macro7` | **Space** | 0–1 | 0.4 | shimmer + long verb send |
| `macro8` | **Weight** | 0–1 | 0.3 | sub + ghost (capped) |

### XY mapping (Nebula6)

| Axis | Low (0) | High (1) |
|------|---------|----------|
| **X Darkness** | Cutoff darker, Hollow ↑ | Cutoff open, Hollow ↓ |
| **Y Sky** | Dry core, chorus down | Chorus ↑, Shimmer ↑, Nebula ↑ |

## Factory programs (12 + Init)

| # | Name | Tag | Intent |
|---|------|-----|--------|
| 0 | Init Nebula | — | Balanced |
| 1 | Cloud Nine Juno | Cloud | Wide shimmer pad |
| 2 | Slowed Halo Pad | Cloud | Long release, tape |
| 3 | Astral Minor Hold | Cloud | Frozen-friendly |
| 4 | Velvet Sub Bed | Cloud | Sub bed, nebula haze |
| 5 | Phonkwave Highway | Phonkwave | Dotted arp, Chorus II |
| 6 | Midnight Drive Chord | Phonkwave | PWM + drift couple |
| 7 | Neon Underpass | Phonkwave | Res + hollow lead |
| 8 | Cryst Juno Stack | Phonkwave | Dual osc stack |
| 9 | Crypt Choir Pad | Atmospheric | Nebula vowel max |
| 10 | Underwater 106 | Atmospheric | Hollow + dark cutoff |
| 11 | Drift Bell Halo | Atmospheric | Ghost low, shimmer high |
| 12 | Tokyo Fog | Atmospheric | Frozen + long tail |

`preset_tag` stored in preset metadata for Dice bounds: `cloud` | `phonkwave` | `atmospheric`.

## MPC port preview (not v1 desktop)

| Melodee K | Maps to |
|-----------|---------|
| K1 | `filter_cutoff` |
| K2 | `nebula_level` |
| K3 | `chorus_mix` |