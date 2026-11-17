# MpcTr808909 — creative brief

**Target:** **TR-808 / TR-909**-style **synthesized** drums for **Melodee** — no static sample ROM. **Pitch-swept sine kick** (808 sub), **bridged-tone snare** (tuned osc + noise), **metallic hat** (square / filtered noise), **cowbell** (detuned squares), optional **909 kick click** layer.

**Play mode:** one plugin instance, **GM drum map** (36 kick, 38 snare, 42 CH, 46 OH, 49 crash optional v2, 56 cowbell, 37 rim, 45/47/48 toms).

**Melodee performance:** **K1–K3** follow **focus voice** (default **kick**): decay, tune/pitch sweep depth, drive. Programs switch focus (Snare Edit, Hat Edit) or kit preset.

**CPU:** each hit is **short mono synth voice** (not 6-voice poly pad) — many simultaneous hits OK on ARM.

**Not** a circuit clone — inspired envelopes and topology in float C++.