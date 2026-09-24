# V Voyager (Voyager-style monophonic VSTi)

Triple oscillator, Moog ladder LPF, dual envelopes, LFO, glide, mixer saturation, PolyBLEP sources.

## Factory preset bank (24)

Full catalog: [PRESETS.md](PRESETS.md) — leads, basses, pads, keys, brass, FX.

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
