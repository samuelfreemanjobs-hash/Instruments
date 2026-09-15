# Phase checklist (from product roadmap)

## Phase 1 — Tech stack & architecture ✅ (foundation)

- [x] C++20 + JUCE 8 via CMake FetchContent
- [x] MVC folders: `Model`, `View`, `Controller`, `Audio`, `App`
- [x] Audio thread isolated in `AudioEngine` with documented RT constraints
- [x] `ProjectState` / `ValueTree` scaffold

## Hybrid modes (Maschine / Ableton / FL / Electribe)

- [x] Mode toolbar + `HybridWorkspace` models
- [x] **Electribe mode default** — 16-step row + 8 parts (`ElectribeSequencer`)
- [x] Maschine pads/scenes, Session clip grid, FL playlist (UI + model stubs)

## Phase 2 — Graphics & UI (in progress)

- [x] `VMpcLookAndFeel` (matte black / walnut / ice-blue)
- [x] `Mpc2000xlLcdDisplay`, `StepSequencerGrid`, `PianoRollGrid`, `MixerChannelStrip`
- [ ] Pixel-match UI mockup layout (Q-Link, full mixer, hardware shell)
- [ ] 7-segment LED font + vector studio emblem assets

## Phase 3 — Sequencer engine (partial)

- [x] `SequencerClock` with PPQN advancement
- [x] `SixteenStepPattern` + `SequencerCore` → MIDI in audio callback
- [ ] Full track list, timestamped events, quantized + unquantized ticks
- [ ] MPC swing applied per-step in clock (helper stub in `SequencerClock`)

## Phase 4 — Audio & DSP

- [ ] Sampler (.SND / .PGM), pitch interpolation, ADSR, filters
- [ ] Transient slicing
- [ ] Unit A/B/C FX + 3-band EQ DSP

## Phase 5 — Integration

- [ ] Q-Link CC automation record/playback
- [ ] `AsyncUpdater` waveform + meter bridge
- [ ] Full project XML/JSON via `ValueTree`

## Phase 6 — Polish

- [ ] Master VU/LUFS, offline bounce, SIMD profiling
