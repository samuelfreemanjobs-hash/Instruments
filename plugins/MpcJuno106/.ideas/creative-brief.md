# MpcJuno106 — creative brief

**Target:** Roland Juno-106 / Juno-60 character for **Akai MPC Sample / Melodee**: one **DCO** per voice (saw + pulse + sub), **24 dB/oct lowpass**, signature **stereo BBD-style chorus**. Optimized for **low CPU** and **rich pads** on embedded ARM.

**Not a ROMpler** — band-limited wavetable/DPW oscillators + lightweight chorus, not cycle-accurate chip clone.

**Host:** Melodee headless; parameters 1–3 map to hardware knobs (cutoff, resonance, chorus mix or rate).

**Polyphony cap:** **6 voices** on Melodee; **8** on desktop builds via compile flag if CPU allows.