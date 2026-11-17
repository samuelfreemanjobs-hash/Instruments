---
name: "drum-voice-recipes"
description: >
  Drum voice and DSP recipes for kick, 808, snare, clap, hats, cowbell, bus chain, aliasing control
  and real-time safety, tuned for drift/Memphis phonk. Use when designing or reviewing a drum
  voice, writing a parameter table, planning an offline render or handing a voice spec to FORGE.
  Do NOT use for kit packaging or naming (kit-product-spec) or file QA (sample-delivery-qa).
version: "1.0.0"
scope: "project"
author: "Sam / Freeman Intelligence"
---

## Must-read constraints

1. Evidence tags: **[SRC]** a source states it, **[CONV]** common practice with no source, **[DERIVED]** arithmetic from sourced facts. Every [CONV] value ships as a tunable preset tagged [A] in outputs, never as fact.
2. Public phonk sources give almost no phonk-specific numbers. Say so; never quote a value from this file as "the standard".
3. Numbers about pitch, dB, glide and loops come from scripts/dsp_math.py.

## Real-time rules (a missed deadline is an audible click)
- No allocation, locks, logging or I/O in processBlock or per-sample code. Preallocate; pass UI values via atomics or lock-free queues. [SRC]
- Smooth every parameter (linear for most, multiplicative for Hz/dB; multiplicative cannot reach zero). Never share one smoother across threads. [SRC]
- Create oversampling objects outside the callback, report latency, compensate it in offline tests. [SRC]
- One ScopedNoDenormals at the top of processBlock. [SRC]
- One shared DSP core for plugin and offline renderer so the sample equals what the plugin plays. [CONV]
- Per-voice seeded PRNG for noise; few-ms fade on note-on and steal; glide in the log (pitch) domain. [CONV]

## Voice recipes (starting points, tune by ear)

| Voice | Recipe | Evidence |
|-------|--------|----------|
| **Cowbell** | Two square oscillators at 540 and 800 Hz (ratio about 1.48), one shared decay envelope, bandpass after the sum. Lock the ratio; move only base pitch, since small deviations break the clang. | [SRC] |
| Cowbell, phonk use | Pitch to the track key, slight detune, distort or bit-crush, control the tail the distortion lengthens (shorter post-drive envelope or multiband release control), remove sub. | [SRC] |
| Cowbell baseline | Two 47-50% pulse oscillators at 1.48, one exponential envelope 400-600 ms, bandpass near the upper partial tracking pitch, drive, bit-crush, clipper, high-pass. PolyBLEP or oversampling for the squares. | [CONV]/[SRC] |
| **808** | Mono legato sine, short pitch dive, glide about 300 ms in one sourced recipe; distort only above the sub; multiband split near 123 Hz with much less drive on the low band; optional parallel grit layer high-passed 0.8-3 kHz; keep mono. | [SRC] |
| 808 paths | Three bands: clean sub, driven mids, grit layer. DC blocker after asymmetric stages. | [DERIVED]/[CONV] |
| 808 glide | Scale to note division, not fixed ms: 300 ms is 1.48 sixteenths at 74 BPM but 2.4 at 120 (dsp_math.glide_sixteenths). Dive depth/time and decay per tempo are unsourced: expose as parameters. | [DERIVED] |
| **Kick** | Kick gives transient, 808 gives body. Sweep 150-250 Hz down to 45-55 Hz over 30-80 ms, 1-3 ms click: design intent only. | [SRC] role / [CONV] numbers |
| **Snare/clap/hats** | Layer snare + clap or rim, bus with EQ and saturation, optional short plate. Backbeat mode switch: half-time (snare/clap on beat 3) vs four-on-the-floor (clap on 2 and 4). Swing sources disagree (5-20%): make it a parameter. Cowbell can replace hats. | [SRC]/[DERIVED] |
| **Bus/master** | Order: per-voice distortion and EQ, bus saturation, glue compression (about 2:1, 1-2 dB reduction), parallel crush, clipper, optional lo-fi, true-peak ceiling. | [CONV] order |

## Nonlinear stages and aliasing
Clippers and saturators alias. Use 2x-8x oversampling, or ADAA (first order leaves residual aliasing; pair with 2x). Put a true-peak limiter after an oversampled clipper because its filter can overshoot. [SRC]

## Parameter table format (output of W2)
| Parameter | Start | Range | Provenance | Tune by ear |
|-----------|-------|-------|------------|-------------|
| cowbell.ratio | 1.4815 | locked | [S] 800/540 [C] partial_ratio | no |
| cowbell.decay_ms | 500 | 400-600 | [A] | yes |
| 808.glide_sixteenths | 1.5 | 0-4 | [A] | yes |
| 808.split_hz | 123 | 80-160 | [S]/[A] | yes |
| master.ceiling_dbfs | -1.0 | fixed | [S] delivery rule | no |

## Honest limits
No public source gives phonk cowbell decay or mix level, 808 dive depth, kick sweep, kick-vs-808 level, bus drive, or a tuning tolerance. State this in outputs. "Original", "tuned" or "standards-compliant" need a passing check and a provenance record.
