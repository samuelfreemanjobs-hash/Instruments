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

## Phase 2 — Graphics & UI ✅ (foundation)

- [x] `VMpcLookAndFeel` (matte black / walnut / ice-blue)
- [x] `Mpc2000xlLcdDisplay`, `StepSequencerGrid`, `PianoRollGrid`, `MixerChannelStrip`
- [x] Hardware shell frame, Q-Link strip, 4-channel mixer bank (MPC mode)
- [x] `SevenSegmentDisplay` for BPM / step readouts
- [ ] Pixel-perfect mockup match (full 16-pad deck, emblem art)
- [ ] Dedicated 7-segment typeface asset (vector paths)

## Phase 3 — Sequencer engine (in progress)

- [x] `SequencerClock` with PPQN advancement
- [x] `SixteenStepPattern` + `SequencerCore` → MIDI in audio callback
- [x] `SequencerSong` / `SequencerTrack` event lists (scaffold)
- [x] MPC swing on Electribe 16th steps (`samplesForSixteenthStep`)
- [ ] Quantized + unquantized tick recording per event
- [ ] Swing on MPC `SequencerCore` path

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
