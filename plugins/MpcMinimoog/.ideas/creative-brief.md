# MpcMinimoog — creative brief

**Target:** **Minimoog Model D** character for **MPC Sample / Melodee** as a **monophonic** bass & lead: **three oscillators**, **noise**, **glide/portamento**, **ladder lowpass** with intentional **saturation** (`juce::dsp::LadderFilter` + drive stage).

**Why mono:** one voice only → budget for **heavier filter nonlinearity** and subtle **oscillator drift** (slow per-osc offset LFO) without stacking 6× CPU.

**Not** a component-level clone — topology and feel (thick bass, screaming lead with high emphasis + drive).

**Host:** headless Melodee; **K1–K3** = **cutoff**, **emphasis** (resonance), **glide** (or filter drive on alt program bank).

**Use case:** default **bass** slot on SD card; optional lead patches via programs.