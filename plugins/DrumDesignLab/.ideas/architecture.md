# DrumDesignLab — architecture (APC Plan)

**Phase:** APC Plan · between DPCS Phase 1 and Phase 2
**Inputs:** [creative-brief.md](creative-brief.md) · [parameter-spec.md](parameter-spec.md) · [../DPCS_PHASE0_CONSTITUTION.md](../DPCS_PHASE0_CONSTITUTION.md)
**Normative:** [dsp-architecture-specification.md](../../../docs/dsp-architecture-specification.md) · [DPCS.md](../../../docs/DPCS.md)
**SOP:** [SOP-VF-04 Plan](../../../docs/disklordz/sops/SOP-VF-04-plan.md)

## 1. Core components

| Component (spec §3.1) | Lives in | Role |
|-----------------------|----------|------|
| `CinematicDSP` (baseline) | `vst/DrumDesignLab/Source/CinematicDSP.h` | Zero-JUCE C++20 7-voice × 5-mood (`Dark`, `Luxury`, `Haunted`, `Aggressive`, `Atmospheric`) baseline synthesizer |
| `ThreeLayerDrum` (core) | `vst/DrumDesignLab/Source/ThreeLayerDrum.h` | Zero-JUCE `LayerRecipe`, `DrumRecipe`, `RuntimeVoice`, `BodyGenerator`, `AttackGenerator`, `TextureGenerator`, `LayerMixer`, `QualityAnalyzer` |
| `HyperDNAMorph` | `vst/DrumDesignLab/Source/DNA/HyperDNAMorph.h` | 5-node 2D XY morph matrix (`Plugg`, `Rage`, `Hypertrap`, `Hyperpop`, `Digicore`) with single-pass log-ratio pitch interpolation & locks |
| `MutationEngine` | `vst/DrumDesignLab/Source/Mutation/MutationEngine.h` | Seeded `xoshiro128++` directed mutation, recipe breeding, layer-isolated PRNG namespaces |
| `GenreBankLibrary` | `vst/DrumDesignLab/Source/Presets/GenreBankLibrary.h` | Cinematic Shadows + Detroit, Plugg, Cloud Bounce, Heavy ATL Brick, Houston Trunk 12-bit, Chicago Drill, Louisiana, Rage, Hyper Series, Memphis, Phonk |
| `StepSequencer` | `vst/DrumDesignLab/Source/Sequencer/StepSequencer.h` | Rational beat-to-sample 16/32/64-step groove scheduler, swing, Louisiana bounce sync, rolls, per-step DNA XY, SMF `.mid` export |
| `WavExporter` | `vst/DrumDesignLab/Source/Factory/WavExporter.h` | Zero-JUCE 16/24-bit PCM + 32-bit float WAV writer/reader, atomic staging, SHA-256 & JSON/CSV manifest |
| `Context` (`PluginProcessor`) | `vst/DrumDesignLab/Source/Plugin/PluginProcessor.h/.cpp` | 16-pad MIDI rack, 32 preallocated `RuntimeVoice` pool, double-buffered POD `KitProgram` snapshot swap, APVTS |
| `Editor` (`PluginEditor`) | `vst/DrumDesignLab/Source/Plugin/PluginEditor.h/.cpp` | Responsive 5-mode DiskLordz dark industrial UI (`SYNTH`, `DESIGN`, `DNA`, `GROOVE`, `FACTORY`), 4x4 pads, 3 layer strips, XY matrix, `DRAG WAV` / `DRAG MIDI` |

### Reuse audit (`shared/dsp/` & `vst/factory-lib/`)

- Existing reusable assets inspected: `shared/dsp/{biquad.h, envelope_adsr.h, parameter_smoothing.h, phasor.h}`, `vst/factory-lib/analysis/{PeakRms.h, MeterBridge.h, MeterBallistics.h}`, and `vst/factory-lib/gui/` (`HeaderBar`, `RotaryKnob`, `LevelMeter`, `PresetBrowser`, `SectionPanel`, `LedToggle`, `ValueReadout`, `generated/disklordz/DesignTokens.h`).
- Zero-JUCE core headers in `vst/DrumDesignLab/Source/` compile standalone for fast `DrumDesignLabCoreTests` and `CinematicRenderer` CLI without requiring JUCE headers, while `PluginProcessor` / `PluginEditor` bridge into JUCE and `disklordz` theme tokens.

## 2. Program chain (audio thread, per `processBlock`)

```text
for each block (numSamples 0..maxBlockSize):
  1. juce::ScopedNoDenormals; if numSamples == 0 return; clear output stereo bus
  2. Read atomic active KitSnapshot index (fixed-size POD double buffer; zero locks, zero heap)
  3. Read & smooth continuous APVTS overrides (10-20 ms linear/log steppers)
  4. If ddl_bypass: ramp active voices over 5 ms -> write 0.0 -> update PeakRms meter -> return
  5. Advance host/internal StepSequencer & Roll Keyswitches (MIDI 24..27) into fixed stack event array
  6. Merge incoming MidiBuffer events sorted by sample offset [0..numSamples-1]:
     - Sub-block render active voices [cursor .. eventOffset)
     - Note-On (pads 36..51 or custom map): choke matching group (2.5 ms linear de-click fade),
       allocate free or oldest/quietest slot from 32-voice pool, init independent Body/Attack/Texture seeds & phases
     - All-Notes-Off / All-Sound-Off: trigger 2.5 ms fast fade on all active voices
  7. Render remaining sub-block [cursor .. numSamples):
     - Per active RuntimeVoice: Body + Attack + Texture -> LayerMixer (mono clean sub < 120 Hz +
       4x oversampled nonlinear Dirt branch + DC blocker + clickless tail fade) -> stereo pan
  8. Apply smoothed ddl_master_gain_db, sanitize non-finite samples to 0.0, hard safety clamp +-4.0f,
     publish lock-free Peak/RMS + active pad LED bitmask to UI
```

## 3. Surface chain (message thread)

| Surface | Binding |
|---------|---------|
| APVTS | All `ddl_*` parameters from `parameter-spec.md` v1 |
| Editor timer (30 Hz) | Polls atomic Peak/RMS meter bridge, pad trigger LED bitmask, and async preview buffer ready flag |
| Async preview & batch worker | Renders waveform preview WAVs, `DRAG WAV` / `DRAG MIDI` managed temp files, and `FACTORY` batch ZIPs off the message & audio threads |
| State | Versioned APVTS XML + compact JSON `KitProgram` child (`schema_version = 1`) |

## 4. Global layer

| Item | Value |
|------|-------|
| Bus layout | Synth: 0 audio in, 1 stereo audio out + MIDI input |
| `prepareToPlay` | Preallocates 32 `RuntimeVoice` slots, oversampler buffers, andoothed parameter steppers |
| `releaseResources` | Resets voice pool and waits for any background preview render cleanly |
| Programs | 21 factory presets led by `Cinematic Shadows` (`Dark`, `Luxury`, `Haunted`, `Aggressive`, `Atmospheric`) + regional/Hyper banks |
| Plugin codes | Manufacturer `Inst`, plugin `DlDL` (verified unique across repo `CMakeLists.txt`) |

## 5. Threading and realtime contract

- **Audio thread may:** read atomics, read active immutable POD `KitProgram` slot, iterate `MidiBuffer`, run bounded fixed-array `RuntimeVoice` DSP, write atomic meter/LED floats.
- **Audio thread must not:** call `new`/`malloc`, resize `std::vector`, destroy last `std::shared_ptr`, lock `std::mutex`, open files, run dynamic FFT plans, or touch JUCE GUI components.

## 6. Build wiring (Implement phase)

- Zero-JUCE core + tests + 35-WAV CLI:
  `cmake -S vst/DrumDesignLab -B vst/DrumDesignLab/build-core -G "Visual Studio 17 2022" -A x64 -DDISKLORDZ_BUILD_JUCE_PLUGIN=OFF`
- Full JUCE VST3 + Standalone (`COPY_PLUGIN_AFTER_BUILD FALSE`, LTO off):
  `cmake -S vst/DrumDesignLab -B vst/DrumDesignLab/build-juce -G "Visual Studio 17 2022" -A x64 -DDISKLORDZ_BUILD_JUCE_PLUGIN=ON -DJUCE_DIR=<local-juce-path>`

## 7. Verification plan (DPCS Phase 4)

| # | Check | How | Exit-0 criterion |
|---|-------|-----|------------------|
| V1 | 35 Cinematic Shadows designs (7 voices × 5 moods) | `ctest -R DrumDesignLabCoreTests` | All 35 finite, non-silent, DC `<= 0.001`, peak bounded, bit-identical deterministic repeat |
| V2 | Layer isolation & seed namespaces | `DrumDesignLabCoreTests` | Locking/muting Attack leaves Body & Texture bit-identical; Solo isolates target layer |
| V3 | 808 tail tuning & clean sub | `DrumDesignLabCoreTests` | Autocorrelation tail within `+-25 cents` across `44.1/48/96 kHz` |
| V4 | Hyper DNA 2D morph & single-pass pitch ratio | `DrumDesignLabCoreTests` | Weights sum to `1.0`, `intensity=0` bit-identical to source, locks preserved |
| V5 | Zero-drift 64-step sequencer & SMF `.mid` | `DrumDesignLabCoreTests` | Step 63 within `+-1 sample` of exact rational position; valid `MThd/MTrk` |
| V6 | 35 WAV batch render & reopen QA | `CinematicRenderer` | Writes 35 `48 kHz / 24-bit` WAVs + `MANIFEST.json` with SHA-256 & 0 QA failures |
| V7 | JUCE VST3 + Standalone compile | `cmake --build build-juce --config Release` | Produces `DrumDesignLab.vst3` & `DrumDesignLab.exe` (`COPY_PLUGIN_AFTER_BUILD FALSE`) |

## 8. Open issues resolved with explicit defaults

- **A. Remote starter ZIP (`CinematicDSP.h` / `ThreeLayerDrum.h`):** Those two headers were referenced from a remote download link and were not extracted on local disk (`DiskLordzDrumFactory/Source/DrumDSP.h` was present in `Downloads` and inspected). Default: implement clean, self-contained zero-JUCE `Source/CinematicDSP.h` and `Source/ThreeLayerDrum.h` fulfilling every contract in Master Prompt §1–§9 while incorporating `DrumDSP.h`'s xorshift/DC-blocker lessons.
- **B. Local JUCE version:** Repo `/JUCE_VERSION` pins `7.0.12` while local `Downloads` has `juce-9.0.2-windows\JUCE` extracted on disk. Default: guard JUCE 7/8/9 minor differences (`FontOptions` vs `Font`, etc.) so `CMakeLists.txt` compiles cleanly against either `-DJUCE_DIR` checkout without touching system folders.

## 9. Phase 2–3 allowlist

- `vst/DrumDesignLab/**`
- `plugins/DrumDesignLab/**`
