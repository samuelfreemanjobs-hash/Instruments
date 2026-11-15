# DrumDesignLab — DPCS Phase 0 Constitution

**Status:** Phase 0 · **Next:** Phase 1 PRD (`.ideas/creative-brief.md`, `.ideas/parameter-spec.md`)
**Class:** synth · **Category:** `synth.subtractive` · **Created:** 2026-11-15
**SOP:** [SOP-VF-01 Intake & Dream](../../docs/disklordz/sops/SOP-VF-01-intake-dream.md)

## Purpose

`DrumDesignLab` is the unified DiskLordz drum synthesis instrument, three-layer sound-design laboratory, and deterministic sample-kit factory powering Detroit Trap, Plugg (`Classic`..`Ambient`), Cloud Bounce, Heavy ATL / Brick Trap, Houston Trunk (`12-bit / 26.04 kHz`), Chicago Drill, Louisiana (`New Orleans Bounce` + `Baton Rouge Trap`), Rage (`Redline`..`Nebula`), the **20-Family Hyper Series** (separate `Hypertrap` and `Hyperpop` engines plus `Hyperplugg`, `Digicore`, `Glitchcore`, `HexD`, `Sigilkore`, `Scenecore`, `Dariacore`, `Digital Jerk`, `New Jazz`, `Dreamtrap`, `Cloud Rap`, `Bubblegum Bass`, `Deconstructed Club`, `Nightcore Trap`, `Cybertrap`, `Electropop Trap`, `Trance Trap`, and `Experimental Digital Trap` driven by a **Hyper DNA Morph** axis), Memphis, Drift Phonk, and Southern Trap without requiring any external recordings or commercial one-shot dependencies. It serves producers who play a 16-pad MIDI drum rack in a DAW or standalone app while mutating or morphing seeded drum recipes, dragging finished WAVs/MIDIs straight into their session, and batch-manufacturing QC-verified commercial kits.

## Scope (in)

- Formats: VST3 + Standalone via `vst/template`
- Operating modes: `SYNTH` (per-instrument synthesis), `DESIGN` (Body / Attack / Texture layering, transient sculptor, seeded mutation, A/B compare), `FACTORY` (deterministic WAV one-shots, MIDI bounce grooves, manifests, ZIP packs)
- Factory-lib DSP modules: `osc.sine`, `osc.saw`, `osc.square`, `osc.wavetable`, `env.adsr`, `lfo.sine`, `filter.ladder`, `filter.biquad`, `filter.hpf`, `fx.saturation`, `fx.delay`, `dyn.compressor`, `analysis.peak_rms`
- Domain synthesis engines (first wave in V1 core, remaining engines gated behind M2+ slices):
  - First-wave core instruments: Bass 808 (phase-coherent clean-sub + oversampled dirt parallel crossover, legato portamento/glide), Kick, Snare, Clap, Hi-Hat (closed/open choke, 1/16, 1/32, triplet & stutter rolls), Rim, FM/Alien Percussion, Rage FX
  - Musical personality packs on one shared DSP core: Detroit Drum Engine, Plugg Drum Engine, Cloud Bounce Drum Engine, Heavy ATL / Brick Trap Engine, Houston Trunk Engine, Chicago Drill Engine, Louisiana Bounce & Baton Rouge Engine, Rage Drum Engine, **Hypertrap Engine** (`Hyper808Reactor`, `DigitalKickCannon`, `RazorHatGen`, `LaserPercDesigner`, `StutterEngine`), **Hyperpop Engine** (`PlasticPercSynth`, `RubberKickEngine`, `GlitchTransientDesigner`, `SyntheticClapStack`, `DigitalDestructionProcessor` — wavefolder, bitcrusher, glitch gate, Hilbert/ring frequency shifter), plus the remaining 18 Hyper Series recipe banks (`Hyperplugg`, `Digicore`, `Glitchcore`, `HexD`, `Sigilkore`, `Scenecore`, `Dariacore`, `Digital Jerk`, `New Jazz`, `Dreamtrap`, `Cloud Rap`, `Bubblegum Bass`, `Deconstructed Club`, `Nightcore Trap`, `Cybertrap`, `Electropop Trap`, `Trance Trap`, `Experimental Digital Trap`), Memphis Drum Engine, Phonk Drum Engine, Southern Trap Engine
  - 12-engine architecture slots: Analog Drum, Hybrid Analog/Digital, FM Drum (2–6 op), Physical Modeling, Additive, Subtractive, Phase Distortion, Karplus-Strong, Modal, Waveshaping/Wavefolding, Spectral Resynthesis (offline/worker-rendered when FFT-based), Procedural Foley
- Factory-lib GUI modules: `gui.pad-grid`, `gui.rotary-knob`, `gui.waveform`, `gui.adsr-panel`, `gui.meter`, `gui.key-selector`, `gui.xy-pad`
- Design-system theme: `disklordz`
- Target OS primary: Windows x64 (VST3 + Standalone); future macOS AU/VST3

## Exclusions (out)

- Bundled third-party or recorded WAV samples of any kind (100% procedural synthesis only)
- Dynamic FFT plan creation, heap allocation, file I/O, or blocking mutexes on the real-time audio callback
- Dynamic brickwall lookahead mastering inside the playable live instrument voice path (export ceiling / true-peak checking belongs to the offline `FACTORY` export pipeline, kept separate from playable pad voice gain)
- Cloud account sync, storefront checkout, or licence-server calls inside the audio plugin binary
- Multi-out DAW bus routing beyond stereo main out in V1 Milestone 1 (stems are rendered offline in `FACTORY` mode)

## Stack

| Layer | Choice |
|-------|--------|
| Framework | JUCE (repo pin in `/JUCE_VERSION`: 7.0.12; JUCE-8-only APIs guarded until repo-wide pin bump is approved) via `vst/template` |
| Spec | `factory-spec/v2`, category `synth.subtractive` |
| UI | Factory design system, theme `disklordz` |
| Build | `tools/build-vst.sh DrumDesignLab` |
| Verify | `run_business.py --profile factory --plugin DrumDesignLab` |
| Methodology | [docs/DPCS.md](../../docs/DPCS.md) |
| Gates | `approve_architecture` before scaffold · `approve_gui` before editor sign-off · `approve_release` before ship |

## Non-negotiables

1. **Component-first** — reusable DSP and GUI live in `vst/factory-lib/`, never duplicated in `Source/`.
2. **DONE = exit 0** — every phase ends with a script that exits 0.
3. **Phase 1 zero code** — behaviour docs only until Phase 2 headers.
4. **One module per Phase 3 run** — explicit file allowlist in each handoff.
5. **Realtime** — preallocated 32-voice pool, immutable recipe/bank snapshots, per-voice runtime DSP state separated from immutable drum recipes, and zero allocation, locks, filesystem I/O, or UI calls in `processBlock`.
6. **Deterministic & phase-coherent** — `renderOneShot` and live voice triggers reset oscillator/hat phases and seed PRNGs from explicit recipe seeds so live audition and offline WAV export match within tolerance; exported WAVs use atomic temp-file writes and managed drag-and-drop file lifetimes.
7. **Theme roles only** — UI cites design-system role names, never hex values.

## Open operator question (recorded for Plan gate)

- **JUCE pin:** Repo `/JUCE_VERSION` is pinned to `7.0.12` while the roadmap prose cites JUCE 8 / C++20. Defaulting to C++20 compatible with the repo's `7.0.12` pin (with `#if JUCE_MAJOR_VERSION >= 8` guards if needed) unless the operator approves bumping `/JUCE_VERSION`.

## Acceptance (Phase 4 targets)

```bash
tools/build-vst.sh DrumDesignLab
python3 vst-testing-ops/run_business.py --profile factory --plugin DrumDesignLab
bash plugins/DrumDesignLab/scripts/verify-phase4.sh
```

## Approvals

Operator notifications: Slack `#disklordz-factory-dept` (see [VST_FACTORY_SLACK_SETUP.md](../../docs/VST_FACTORY_SLACK_SETUP.md)).
