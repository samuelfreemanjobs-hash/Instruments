# Huckaby Pattern Engine — architecture

## Purpose

**Huckaby Pattern Engine** is a JUCE-based **drum pattern / sampler instrument** that models the “decoupled rhythm + hot-swappable banks + polyrhythmic loop lengths” workflow described in the Mike Huckaby method notes. It ships as **VST3 + Standalone** targets in this monorepo.

Primary goals:

- Keep groove steps stable while **timbral banks** can be swapped underneath.
- Allow each lane to run a **different loop length** so composite patterns drift in phase (LCM-based evolution).
- Provide bank indexing modes: **static**, **modulo-on-trigger-count**, and **deterministic random**.

## Build & run

From the monorepo root:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target HuckabyPatternEngine_Standalone HuckabyPatternEngine_VST3
```

The standalone binary is produced next to other JUCE artifacts under the CMake build output tree (platform-dependent).

## Data flow

1. **Transport / timing**
   - The sequencer advances on **16th-note steps** derived from BPM.
   - **Internal transport mode** uses the `internal_bpm` + `internal_play` parameters.
   - When internal transport is off, the sequencer follows the host playhead (`AudioPlayHead`).

2. **Pattern layer**
   - Each track stores a compact **bitmask** of enabled steps (`uint64_t`), edited in the UI.
   - Per-track parameters control **loop length**, **bank selection mode**, **static bank index**, and **modulo divisor**.

3. **Hot-swappable sample banks**
   - Each track has **16 bank slots** (`AtomicSharedSample`), each holding `std::shared_ptr<SampleData>`.
   - Sample decode happens on a **background thread pool**; installation into the bank happens on the **message thread** via `MessageManager::callAsync`, so the audio thread never allocates or decodes.

4. **Rendering**
   - On a trigger, a `DrumVoice` captures a `shared_ptr` snapshot and mixes linearly-interpolated playback into the output buffer.

## Threading / realtime constraints

- **Audio thread (`processBlock`)**: DSP + sequencer evaluation + voice mixing only.
- **Background pool (`juce::ThreadPool`)**: disk read + decode + `AudioBuffer` construction.
- **Message thread**: pointer installation into atomics + UI.

This avoids mutex contention on the audio thread while still allowing safe lifetime via `shared_ptr`.

## Key modules

| Path | Responsibility |
|---|---|
| `Source/PluginProcessor.*` | Transport, APVTS parameters, sequencer tick, MIDI hook (program change placeholder), state save/restore |
| `Source/TrackModel.h` | Track state, default demo patterns, bank sizing constants |
| `Source/SampleTypes.h` | `SampleData` + `AtomicSharedSample` |
| `Source/DrumVoice.h` | One-shot interpolated playback voice |
| `Source/PluginEditor.*` | Tabs per track: pattern bits, bank controls, WAV loader |

## Extension points

- **MIDI program changes**: `processIncomingMidi` is prepared as the seam for “scene/bank preset” automation.
- **Additional layers**: add a secondary per-track `AtomicSharedSample` bank for “texture under transient” layering.
- **Probability / velocity humanization**: extend trigger evaluation before allocating voices.

## Related docs

- Monorepo index: `/ARCHITECTURE.md`
- JUCE testing ops (if integrated into CI later): `vst-testing-ops/ARCHITECTURE.md`
