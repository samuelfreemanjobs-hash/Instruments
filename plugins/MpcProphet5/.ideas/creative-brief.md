# MpcProphet5 — creative brief

**Target:** Sequential **Prophet-5** voicing for Melodee / desktop Linux: **dual analog-style oscillators** per voice, **osc sync**, **Poly-Mod** (osc B → filter cutoff / osc A freq), **4-pole resonant lowpass**. Brass, bass, sync leads.

**Scope v1:** 6 voices; no full Curtis chip emulation — `juce::dsp` oscillators + nonlinear 4-pole or ladder approximation.

**Relation:** `MpcAnalogSynth` is an early subset (2 osc + SVF, no sync/Poly-Mod); this SKU extends or supersedes for Prophet identity.

**Host:** headless Melodee; K1–K3 = cutoff, resonance, osc B level (or sync blend).