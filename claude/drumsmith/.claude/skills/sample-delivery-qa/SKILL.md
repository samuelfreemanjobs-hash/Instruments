---
name: "sample-delivery-qa"
description: >
  Verifies rendered drum samples with scripts/verify_kit.py and dsp_math.py: bit depth, sample
  rate uniformity, peak ceiling, clipping, fades, DC offset, loop length, approximate true peak.
  Use after any render, before any "ready" claim, and for kit audits. Do NOT use to judge tone,
  mix balance or feel; those need Sam's ears.
version: "1.0.0"
scope: "project"
author: "Sam / Freeman Intelligence"
---

## Must-read constraints

1. DONE = `python3 scripts/verify_kit.py <dir>` exits 0 AND CALIPER returns PASS for T3 claims.
2. Run `python3 scripts/self_test.py` first if the scripts changed. It must exit 0.
3. WARN does not fail the run but must be reported. Exit codes: 0 no FAIL, 1 at least one FAIL, 2 no WAVs found.

## Delivery profile "splice"
| Check | Rule | Status |
|-------|------|--------|
| bit_depth | 24-bit PCM WAV | FAIL |
| sample_rate | at least 44.1 kHz, uniform across the pack | FAIL |
| peak_ceiling | sample peak at or below -1.0 dBFS (+0.01 tolerance) | FAIL |
| normalized | peak no quieter than -1.5 dBFS | WARN |
| no_clipping | fewer than 3 consecutive full-scale samples | FAIL |
| edges_faded | first and last sample under 0.01 linear (micro-fade under 3 ms) | FAIL |
| dc_offset | mean under 0.001 pass, to 0.005 warn, above fail | FAIL/WARN |
| loop_length | filename `<n>bpm ... <m>bar` gives expected length within 1 sample | FAIL |
| true_peak | approx above -0.5 dBTP | WARN (approximate meter) |

## Process
1. Run verify_kit.py on the kit folder; keep the JSON (`--json`) in logs/ for the audit trail.
2. Pitched files: measure settled sustain with a window holding several cycles (100 ms at 49 Hz is 4.9 cycles via dsp_math.cycles_in_window). Zero-crossing is valid for clean sub sines only; cross-check with FFT or YIN, since sub-bass can octave-error. Working target 5 cents [A].
3. Loops: rendered length vs `dsp_math.loop_samples`; at odd BPMs a bar is not a whole number of samples, so render to the rounded count.
4. Bit-identical re-render across block sizes and near-zero DC are plugin-side checks (FORGE).
5. Report per file; list what only ears can judge.

## What this cannot judge
Tone, tuning feel, mix balance, whether a cowbell cuts, aliasing audibility. It also does not meter loudness (LUFS) or certify true peak.
