# DrumDesignLab — APC plan

**Architecture:** [architecture.md](architecture.md) · **Next:** `/apc-design DrumDesignLab`
**SOP:** [SOP-VF-04 Plan](../../../docs/disklordz/sops/SOP-VF-04-plan.md)

## Complexity assessment (spec §8)

| Dimension | Score (0–3) | Notes |
|-----------|-------------|-------|
| Voices | 3 | 32-voice polyphonic pool, 16 pads, choke groups, voice stealing with 2.5 ms de-click fade |
| Oscillators/voice | 3 | 3 independent layers (Body + Attack + Texture): polyBLEP, sine/tri, 2–4 op FM, 6-osc metallic, modal partials, seeded noise |
| Filters | 3 | Independent per-layer TPT SVF filters + 120 Hz clean-sub/dirt split crossover + DC blocker |
| Modulation | 3 | Multi-stage pitch envelopes, portamento glide, velocity-to-timbre, 5-node 2D Hyper DNA XY morph, per-step DNA motion |
| FX | 3 | 4x oversampled waveshaper/wavefolder, 12-bit/26.04 kHz vintage sampler grit, transient shaper, stereo width |
| Surfaces | 3 | 5-mode responsive UI (`SYNTH`, `DESIGN`, `DNA`, `GROOVE`, `FACTORY`), 4x4 pad grid, oscilloscope, XY pad, native `DRAG WAV`/`DRAG MIDI` |
| Targets | 2 | Zero-JUCE C++20 core + CLI renderer + Windows x64 VST3 + Standalone |
| **Total** | **20 / 21** | Full flagship instrument + sample-kit factory |

**Tier:** L3 (14–21)

**Risk flags (§8.3):** high polyphony × oversampling CPU; lock-free patch handoff; async external file drag lifetime; multi-rate filter stability.

**Plugin-specific risks**

| Risk | Mitigation |
|------|------------|
| 32 voices × 4x oversampling CPU spike | Apply 4x oversampling on per-voice nonlinear Dirt branch only while voice envelope > `-72 dB`, with early tail termination and zero-allocation fixed buffers |
| Knob/XY edits racing with `processBlock` | Double-buffered POD `KitProgram` snapshots published via `std::atomic<int>` slot index + smoothed scalar overrides |
| External DAW drag deleting temp WAV early | Managed session staging directory retained until plugin destruction / explicit cleanup policy |
| Cumulative step-sequencer timing drift | Compute sample position directly from `(stepIndex * samplesPerBeat) / 4.0` in `double`, never incremental integer addition |

## UI framework

| Field | Value |
|-------|-------|
| `ui_framework` | `juce-native` |
| Design-system theme | `disklordz` |
| Rationale | Zero WebView runtime dependency on Windows DAWs, native `performExternalDragDropOfFiles` support, deterministic 60 fps oscilloscope & XY pad |
| Implementation strategy | Zero-JUCE C++20 DSP core + responsive `juce::FlexBox`/`Grid` dark industrial DiskLordz editor |

## Phase order from here

| Step | Command / artifact | Gate |
|------|--------------------|------|
| Design | `.ideas/design-spec.md` | `tools/apc-gate.sh DrumDesignLab design` |
| Implement M1 | Zero-JUCE `CinematicDSP.h` + `ThreeLayerDrum.h` + `HyperDNAMorph.h` + 35 WAVs + `ctest` | `tools/apc-gate.sh DrumDesignLab impl` + `ctest` exit 0 |
| Implement M2 | JUCE VST3 + Standalone shell (`PluginProcessor`, `PluginEditor`) | `cmake --build` exit 0 |
