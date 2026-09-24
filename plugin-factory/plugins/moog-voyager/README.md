# V Voyager (Voyager-style monophonic VSTi)

Triple oscillator, Moog ladder LPF, dual envelopes, LFO, glide, mixer saturation, PolyBLEP sources.

## Factory preset bank (300 Moog programs)

Full catalog: [PRESETS.md](PRESETS.md) — 32 curated (incl. P-Funk, 80s boogie/R&B, DJ Screw, Troutman) + 268 variants.

Regenerate: `python3 scripts/generate_factory_presets.py`

- DAW: use host **program/preset** (24 programs).
- UI: **Factory preset** dropdown under the title bar.

Default program: **01 Voyager Init**.

## Build & ship

```bash
cd plugin-factory && ./scripts/factory.sh release
```

## Demos

```bash
cmake --build build --target VoyagerLeadDemo VoyagerBassDemo VoyagerPresetTests
./build/plugins/moog-voyager/VoyagerPresetTests
```
