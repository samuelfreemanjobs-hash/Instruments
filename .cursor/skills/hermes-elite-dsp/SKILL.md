---
name: hermes-elite-dsp
description: Senior realtime DSP for poly synth plugins — voice pools, filters, chorus, no audio-thread heap. Use for Junova-X Source/DSP work.
---

# Hermes elite DSP

1. Read `Junova-X/docs/junova-x-mvp-gap-analysis.md` and `Source/DSP/`.
2. Hard rules: no allocation/locks in `processBlock`; smoothed parameters on control rate.
3. MVP smoke: dual ADSR, HPF, chorus Off|I|II, diag tone, MIDI voice stub → full DCO/VCF/BBD later.
4. Scope FIFO for UI: push mono peaks from audio thread only (fixed buffer, atomics).
5. After substantive DSP change: offline sanity + note in PR; golden only when WO requests.

Reference: `Wave909/Source/DSP/`, `Source/DSP/` (JD — do not link binaries).
