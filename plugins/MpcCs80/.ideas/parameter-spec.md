# MpcCs80 — parameter spec (v1)

Melodee **K1–K3** = first three APVTS params (cutoff, resonance, layer balance).

## Global

| ID | Range | Default | Notes |
|----|-------|---------|-------|
| cutoff | 20–16000 Hz | 2800 | Master LPF cutoff (both layers, + per-voice AT) |
| res | 0–0.95 | 0.42 | LPF resonance |
| layerBalance | 0–1 | 0.5 | 0 = Layer I only, 1 = Layer II only, 0.5 = even |
| glide | 0–0.8 s | 0.12 | Portamento |
| aftertouchAmt | 0–1 | 0.65 | Poly AT → cutoff depth |
| pwmRate | 0–1 | 0.35 | LFO depth to PWM (Layer I) |
| pwmRate2 | 0–1 | 0.25 | Layer II PWM LFO |
| ringAmt | 0–1 | 0.0 | Ring mod mix (per active layer) |
| subLvl | 0–1 | 0.4 | Sub-osc level |
| hpfCutoff | 40–4000 Hz | 180 | Shared HPF |
| fAttack / fDecay / fSustain / fRelease | ADSR | brass pad | Filter env |
| aAttack / aDecay / aSustain / aRelease | ADSR | pad | Amp env |

## Per-layer (factory programs bake; no UI overload)

Layer I — `l1wave`, `l1pwm`, `l1detune`  
Layer II — `l2wave`, `l2pwm`, `l2detune` (often softer / string-biased)

Programs encode brass vs. string layer splits; host program change on Melodee.