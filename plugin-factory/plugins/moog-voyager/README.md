# V Voyager (Four-Stack Moog monosynth)

**VOYAGER-V** combines four keyboard lineages in one engine: **Moog Voyager**, **Sub Phatty**, **Subsequent 37**, and **Minimoog Model D**. Triple oscillator, sub osc, Moog ladder LPF, dual envelopes, LFO, glide, mixer saturation, PolyBLEP sources.

- **Four-stack guide:** [docs/MOOG_FOUR_STACK.md](docs/MOOG_FOUR_STACK.md)
- **UI:** hero [docs/UI_CONCEPT_VOYAGER-V.png](docs/UI_CONCEPT_VOYAGER-V.png) · design system [docs/UI_DESIGN_SYSTEM.md](docs/UI_DESIGN_SYSTEM.md) · [agents/UI_AGENT.md](agents/UI_AGENT.md)
- **Preset agent:** [agents/PRESET_AGENT.md](agents/PRESET_AGENT.md)

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
