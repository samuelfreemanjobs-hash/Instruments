# MpcLaSynth — creative brief

**Target:** **Korg M1 / Roland D-50** “sample + synth” hybrid for **Melodee**: **short attack transients** (PCM one-shots) **layered with** looping **synth bodies** (wavetable / harmonic loops). 90s house piano, airy choir pad, Memphis / G-funk bells, stacked brass-ish attacks.

**v1 scope (embedded-safe):**

- **No disk streaming** — attacks and loops shipped as **small embedded PCM** (`BinaryData` or `constexpr` tables, <200 KB total).
- **2 partials per voice max:** **Attack** (one-shot, ~50–300 ms) + **Body** (looping wavetable).
- **8 voices** on Melodee (16 on desktop if profile allows).

**Not** a full ROMpler or M1 ROM clone — curated factory waves + programs.

**Host:** headless; **K1–K3** = **body level**, **attack level**, **filter cutoff** (or attack select macro).