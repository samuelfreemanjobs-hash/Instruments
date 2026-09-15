# Resonance

**Resonance** is a hybrid groovebox DAW from **Instruments** — Electribe-style sequencing, MPC-style pads, multi-mode workflows, Vibe Mixing, VST hosting, and per-channel inserts.

C++20 / JUCE, cross-platform (macOS, Windows, Linux).

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/Resonance_artefacts/Release/Resonance
```

On Linux you need ALSA, X11, FreeType, and OpenGL development packages (see CI or `docs/ARCHITECTURE.md`).

## Layout

- `Source/Model` — sequencer state, PPQN clock, `ValueTree` project scaffold
- `Source/View` — custom components (`StudioLcdDisplay`, `StepSequencerGrid`, `PianoRollGrid`, `MixerChannelStrip`) and `StudioLookAndFeel`
- `Source/Controller` — `AppController` (message thread only)
- `Source/Audio` — `AudioEngine` real-time callback (no UI, no allocation in hot path)

## Roadmap

See [docs/ROADMAP.md](docs/ROADMAP.md).

## Status

| Area | State |
|------|--------|
| JUCE app shell | Yes |
| MVC separation | Yes |
| VST3 / AU / LV2 hosting | [docs/VST_HOSTING.md](docs/VST_HOSTING.md) |
| Resonance internal mix + Vibe Mixing | [docs/INTERNAL_PLUGINS.md](docs/INTERNAL_PLUGINS.md), [docs/VIBE_MIXING.md](docs/VIBE_MIXING.md) |
| Per-channel inserts + project mix save | [docs/MIXING.md](docs/MIXING.md) |

## Brand

Product name and usage: [docs/BRANDING.md](docs/BRANDING.md).
