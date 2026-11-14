# Huckaby Pattern Engine — architecture

## Purpose

**Huckaby Pattern Engine** is a JUCE-based **drum pattern / sampler instrument** that models the “decoupled rhythm + hot-swappable banks + polyrhythmic loop lengths” workflow described in the Mike Huckaby method notes. It ships as **VST3 + Standalone** targets in this monorepo.

Primary goals:

- Keep groove steps stable while **timbral banks** can be swapped underneath.
- Allow each lane to run a **different loop length** so composite patterns drift in phase (LCM-based evolution).
- Provide bank indexing modes: **static**, **modulo-on-trigger-count**, and **deterministic random**.
- **Swap MIDI patterns per track** from a folder of `.mid` files (one bar, PPQ timebase) for rapid “infinite loop idea” iteration, alongside **SWAP samples** in real time.

## Build & run

From the monorepo root:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target HuckabyPatternEngine_Standalone HuckabyPatternEngine_VST3
ctest --test-dir build -R HuckabyPatternEngine_Smoke --output-on-failure
```

The standalone binary is produced next to other JUCE artifacts under the CMake build output tree (platform-dependent).

## Pattern library (MIDI files)

Each track reads **sorted** `.mid` / `.midi` files from a folder:

- **Default (auto-created on first launch):**  
  `{userApplicationDataDirectory}/HuckabyPatternEngine/MidiPatterns/track_1` … `track_8`  
  Factory seeds are generated if folders are empty (stochastic one-bar GM-style note-ons).

- **Import format:** Type 0/1 SMF with **PPQ ticks** (not SMPTE). First bar only: note-on times map to **16th-note steps** modulo one 4/4 bar. Velocities are merged into per-step gain (packed for state save).

- **UI:** per-track tab — **MIDI folder**, **Pat ◀ / Pat ▶**, **Reload**, **Export .mid**. Use any DAW to build a library: export one-bar MIDI clips into the track folder, then flip patterns live.

- **Hardware / MIDI remote:** per-track **Pat CC** and **Pat ch** parameters.  
  - CC value **≥ 64** → next pattern; **< 64** → previous.  
  - **Channel 0** in the parameter means “any channel”.  
  - **MIDI note** `C1` + `D1` … → tracks 1–8: note-on advances that track’s pattern (optional channel filter same as Pat ch).

## Data flow

1. **Transport / timing**
   - The sequencer advances on **16th-note steps** derived from BPM.
   - **Internal transport mode** uses the `internal_bpm` + `internal_play` parameters.
   - When internal transport is off, the sequencer follows the host playhead (`AudioPlayHead`).

2. **Pattern layer**
   - Each track stores a compact **bitmask** of enabled steps (`uint64_t`), edited on a **piano-roll style strip** (horizontal = step time, vertical bar height = hit velocity) or **loaded from MIDI**.
   - Per-step velocities are stored in **four atomic bit-packs** (`VelocityPacking`) for safe RT reads + compact project state.
   - Per-track parameters control **loop length**, **bank selection mode**, **static bank index**, and **modulo divisor**.

3. **Hot-swappable sample banks**
   - Each track has **16 bank slots** (`AtomicSharedSample`), each holding `std::shared_ptr<SampleData>`.
   - Sample decode happens on a **background thread pool**; installation into the bank happens on the **message thread** via `MessageManager::callAsync`, so the audio thread never allocates or decodes.

4. **Rendering**
   - On a trigger, a `DrumVoice` captures a `shared_ptr` snapshot and mixes linearly-interpolated playback into the output buffer.

## Threading / realtime constraints

- **Audio thread (`processBlock`)**: DSP + sequencer tick + voice mixing + **queueing** MIDI pattern advance requests (no file I/O).
- **Message thread**: load `.mid` from disk, update pattern bits + velocities, APVTS + UI sync.
- **Background pool (`juce::ThreadPool`)**: WAV decode + buffer construction.

## Key modules

| Path | Responsibility |
|---|---|
| `Source/PluginProcessor.*` | Transport, APVTS, sequencer, MIDI pattern CC/note, state save/restore |
| `Source/MidiPatternLibrary.*` | Scan folder, import/export SMF, factory pattern seed |
| `Source/VelocityPacking.h` | Pack per-step velocities for atomic RT read + small state |
| `Source/TrackModel.h` | Track state, default demo patterns, bank sizing constants |
| `Source/SampleTypes.h` | `SampleData` + `AtomicSharedSample` |
| `Source/DrumVoice.h` | One-shot interpolated playback voice |
| `Source/PluginEditor.*` | Tabs per track: step piano roll, MIDI library, bank controls, WAV loader |

## Extension points

- Map **note number → step** in MIDI import for multi-resolution patterns.
- **Program change** or **scene** messages to swap entire kits + pattern sets.
- **Probability / humanization** on triggers before voice allocation.

## Related docs

- Monorepo index: `/ARCHITECTURE.md`
- JUCE testing ops (if integrated into CI later): `vst-testing-ops/ARCHITECTURE.md`
