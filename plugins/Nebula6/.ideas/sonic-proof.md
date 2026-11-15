# Nebula6 — sonic proof checklist (remarkable bar)

Use at **design** and **test** gates—not subjective “sounds cool.”

## A. Hero motion

| Step | Action | Pass if |
|------|--------|---------|
| 1 | Preset **Astral Minor Hold**, Cm7 held | Clean stereo image |
| 2 | `frozen_latch` ON | No pitch wander on fundamental |
| 3 | `macro2` Drift 0 → 1 over 4 s | Audible width + tape motion; peak stable ±1 dB |
| 4 | `macro1` Sky 0 → 1 | Shimmer/chorus bloom; no feedback runaway |

## B. Genre lanes

Render 4 s offline each: **Cloud Nine Juno**, **Phonkwave Highway**, **Crypt Choir Pad**.

- Cloud: HF air > −40 dBFS band 4–12 kHz; sub bed present but < −12 dB vs mid
- Phonkwave: arp MIDI audibly dotted; filter movement on cutoff env
- Atmospheric: Nebula bus ≥ −18 dB vs core in L/R correlation < 0.85 (wide)

## C. Stack test

Nebula6 **Velvet Sub Bed** + reference `PhonkCowbell94` cowbell loop (if available):

- Sum bus no harsh 200–350 Hz buildup (> 6 dB peak vs solo beds) → fail Hollow/Weight macro defaults

## D. Regression vs MpcJuno106 core

When shared `juno` DSP lands: same cutoff/res curve within 0.5 dB @ 1 kHz sine sweep (voice solo).

## E. CI contract

`--render-test` (or vst-testing-ops golden) on three tag presets: peak −24 to −0.1 dBFS, no NaN/Inf.