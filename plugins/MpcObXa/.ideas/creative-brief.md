# MpcObXa — creative brief

**Target:** Oberheim **OB-Xa / SEM** color: **dual detuned oscillators**, **12 dB/oct state-variable filter** (band/low/high emphasis), wide **stereo detune** pads, aggressive filter sweeps. **6 voices** on Melodee.

**Filter:** `juce::dsp::StateVariableTPTFilter` with mode switch (LP/BP/HP) — same family as `MpcAnalogSynth`, extended with dual osc detune and cross-pan.

**Host:** headless; K1–K3 = cutoff, resonance, detune spread.