# VMPC2000XL Modern Hybrid DAW

C++20 / JUCE foundation for a cross-platform MPC2000XL-inspired **hybrid DAW** combining workflows from **Electribe** (default step-sequencer mode), **MPC**, **Maschine**, **Ableton Session**, and **FL Studio patterns**. See [docs/HYBRID_FEATURES.md](docs/HYBRID_FEATURES.md).

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/VMPC2000XL_artefacts/Release/VMPC2000XL
```

On Linux you need ALSA, X11, FreeType, and OpenGL development packages (see CI or `docs/ARCHITECTURE.md`).

## Layout

- `Source/Model` — sequencer state, PPQN clock, `ValueTree` project scaffold
- `Source/View` — custom components (`Mpc2000xlLcdDisplay`, `StepSequencerGrid`, `PianoRollGrid`, `MixerChannelStrip`) and `VMpcLookAndFeel`
- `Source/Controller` — `AppController` (message thread only)
- `Source/Audio` — `AudioEngine` real-time callback (no UI, no allocation in hot path)

## Roadmap

See [docs/ROADMAP.md](docs/ROADMAP.md) for the full multi-phase plan (DSP, Q-Link, export, etc.).

## Status

| Area | State |
|------|--------|
| JUCE app shell | Yes |
| MVC separation | Yes |
| 16-step UI + model | Yes |
| VST3 / AU / LV2 hosting | 8-slot rack, scan & native UI ([docs/VST_HOSTING.md](docs/VST_HOSTING.md)) |
| Internal mix plug-ins | Gain, Pan, EQ-3, Compressor, Limiter ([docs/INTERNAL_PLUGINS.md](docs/INTERNAL_PLUGINS.md)) |
| Built-in sampler / FX | Use VST instruments/effects in the rack (Phase 4 internal DSP optional) |
| Project save/load | ValueTree defaults only |
