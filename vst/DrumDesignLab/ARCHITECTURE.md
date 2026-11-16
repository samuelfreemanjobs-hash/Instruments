# DiskLordz Drum Design Lab & Percussion Forge Pro — ARCHITECTURE

## 1. Purpose

`DrumDesignLab` is a Windows-first C++20 percussion synthesis workstation (optional JUCE VST3/Standalone). It houses **Cinematic Shadows**, **Percussion Forge**, **Hyper DNA**, **Hitmaker Multi-808 Fusion**, **Melody & Motion**, **Opium/Carti**, **Drake/Future OVO**, **SGP/Raider**, **Juicy SP-1200**, **Shawty Redd**, **Tay Keith Memphis Club**, **DJ Toomp / Trap Muzik ATL**, **Purps / 808 Mafia**, and regional banks. Every sound is generated mathematically with **zero third-party WAV libraries**.

## 2. Build & Run

### Zero-JUCE Core, Unit Tests & Offline Batch Renderer (fast standalone C++20)

```powershell
cmake -S vst/DrumDesignLab -B vst/DrumDesignLab/build-core -G "Visual Studio 17 2022" -A x64 -DDISKLORDZ_BUILD_JUCE_PLUGIN=OFF
cmake --build vst/DrumDesignLab/build-core --config Release
ctest --test-dir vst/DrumDesignLab/build-core -C Release --output-on-failure
.\vst\DrumDesignLab\build-core\Release\DiskLordzBatchRenderer.exe C:\dev\Instruments-drum-design-lab\vst\DrumDesignLab\renders
```

### Full JUCE VST3 + Standalone Desktop Application

```powershell
cmake -S vst/DrumDesignLab -B vst/DrumDesignLab/build-juce -G "Visual Studio 17 2022" -A x64 -DDISKLORDZ_BUILD_JUCE_PLUGIN=ON -DJUCE_DIR="C:/Users/user/Downloads/juce-9.0.2-windows/JUCE"
cmake --build vst/DrumDesignLab/build-juce --config Release --target DrumDesignLab_VST3 DrumDesignLab_Standalone
```

- `COPY_PLUGIN_AFTER_BUILD` is forced `FALSE` so builds never mutate `C:\Program Files\Common Files\VST3` without explicit operator action.
- VST3 output: `vst/DrumDesignLab/build-juce/DrumDesignLab_artefacts/Release/VST3/DiskLordz Drum Design Lab.vst3`
- Standalone output: `vst/DrumDesignLab/build-juce/DrumDesignLab_artefacts/Release/Standalone/DiskLordz Drum Design Lab.exe`

## 3. Data Flow

```text
MIDI / Sequencer / Roll Scheduler (sample-accurate offsets in block)
        │
        ▼
DrumVoiceManager (32 preallocated ThreeLayerVoice slots, choke groups, 2.5 ms de-click steal)
        │
        ├── Layer 1: BODY     (PolyBLEP / FM / MembraneModel / ModalBank + ShellResonator, own seed)
        ├── Layer 2: ATTACK   (ProceduralExciter: Soft/Hard Mallet, Stick, Felt, Brush, Multi-Impulse, own seed)
        └── Layer 3: TEXTURE  (MetallicResonator / 6-Osc Inharmonic / Colored Noise / Rattle, own seed)
                │
                ▼
        Phase-Aware LayerMixer (Mute / Solo / Gain / Pan + <120 Hz Mono Clean Sub / 4x Oversampled Dirt Split + DC Blocker)
                │
                ▼
        Algorithmic RoomReverb (Early Reflections + Bounded Damped Schroeder Comb/Allpass) + Master Trim
                │
                ├──► Live Audio Bus (VST3 / Standalone) + Atomic Peak/RMS Meter Bridge
                └──► OfflineRenderer ─► AudioQualityAnalyzer ─► Atomic WAV (16/24/32f) / SMF MIDI / MANIFEST.json
```

## 4. Threading & Real-Time Contract

- **Audio thread (`processBlock`):** Reads `std::atomic<int>` active slot index into a fixed-size 2-slot POD `KitSnapshot` double buffer, reads atomic APVTS floats through sample-rate-aware smoothers, schedules MIDI + `OrchestralRollEngine` + `StepSequencer` into stack-allocated `std::array` event buffers, and renders the preallocated 32-voice `DrumVoiceManager`. **Zero heap allocation, zero mutex locks, zero file/disk I/O, zero `std::vector` resizing, and zero UI access.**
- **Message / Worker threads:** Handle recipe editing, 2D `HyperDNA` / `PercussionDNA` weight UI previews, `MutationLab` history/undo, waveform preview rendering, atomic `.tmp`-to-`.wav`/`.mid` staging for `performExternalDragDropOfFiles`, and multi-hundred-sample pack generation.

## 5. Key Modules

| Path | Responsibility |
|------|----------------|
| `Source/DSP/CoreMathAndPRNG.h` | `Xoshiro128pp` layer-isolated PRNG, `PolyBLEP`, `TptSvf`, `OnePoleDcBlocker`, `OversampledWaveshaper4x`, `RoomReverb` |
| `Source/DSP/PhysicalModels.h` | `ModalOscillator`, `ModalBank`, `MembraneModel`, `ShellResonator`, `MetallicResonator`, `ProceduralExciter` |
| `Source/CinematicDSP.h` | 7 Cinematic Shadows voices × 5 moods (`Dark`, `Luxury`, `Haunted`, `Aggressive`, `Atmospheric`) baseline engine |
| `Source/ThreeLayerDrum.h` | `LayerRecipe`, `DrumRecipe`, `ThreeLayerVoice`, 7 Percussion Forge Pro engines (`Timpani`..`CinematicImpact`), `DrumVoiceManager` |
| `Source/DSP/Multi808FusionReactor.h` | Hitmaker flagship: Clean Sub + Punch + Dirt (HP crossover, 4× OS waveshape, phase-aligned trigger) |
| `Source/Presets/HitmakerBank.h` | 8 Hitmaker DNA banks + kit map into ThreeLayer + Fusion |
| `Source/DSP/ShawtyReddEngine.h` | 2005 ATL Shawty Redd — lean 808, snappy snare, hat choke |
| `Source/DSP/TayKeithMemphisEngine.h` | Memphis club trap — boomy driven 808, snare+clap stack, 8th hats + pre-snare stutters, rim/crash (148 BPM bounce) |
| `Source/DSP/ProceduralAtlantaKit.h` | Prepare-time baker: 8 Atlanta pads (808/kick/snare/clap/hats/rim/crash) into stereo buffers — zero RT alloc |
| `Source/DSP/ToompDrumEngine.h` | DJ Toomp — plays baked pads + pitch-env 808, MPC sat, hat choke; `loadPadInterleaved` for custom WAVs |
| `Source/DSP/Purps808MafiaEngine.h` | Purps / 808 Mafia — hard-clipped 45 Hz 808, hybrid DMX snare, dark LP perc, 165 BPM half/double-time bounce |
| `Source/DSP/MultiGenreDrumEngine.h` | Kit Style Selector facade (Hitmaker / MelodyMotion / Opium / OVO / SGP / Juicy / Shawty / TayKeith / Toomp / Purps / Cinematic) |
| `Source/Export/WavExporter.h` | 16-bit stereo WAV writer + peak/DC/NaN QA |
| `Source/DNA/HyperDNAMorph.h` | 5-node 2D XY morph + single-pass log ratio & locks |
| `Source/Lab/IntelligentDrumLab.h` | Prompt → deterministic recipe batch → QA/rank/name (AI models optional, off-RT) |
| `Source/Presets/GenreBankLibrary.h` | 32 DNA formula catalog (Bang / Hitmaker / MelodyMotion / SouthernTriumph) |
| `Source/Factory/KitFactory.h` | Offline kit pack exporter (family folders, variations, MANIFEST.json) |
| `Source/Factory/SoundEditBridge.h` | Focused-pad / voice-family sound edits → DrumRecipe or genre-engine params |
| `Source/Plugin/PluginProcessor.*` | JUCE shell — kit style, pad select, APVTS sound knobs → engine |

## 6. Extension Points

- Add new genre recipe banks in `Source/Presets/GenreBankLibrary.h` using existing `DrumRecipe` POD fields.
- Add new resonator mode tables in `Source/DSP/PhysicalModels.h` (`ModalBank` / `MetallicResonator`).
- Add new APVTS parameters only by updating `plugins/DrumDesignLab/.ideas/parameter-spec.md` first, keeping existing `ddl_*` IDs immutable.
- Intelligent Lab: swap keyword `parsePrompt` for LLM→JSON; optional Stable Audio textures only as offline Texture-layer seeds — see `docs/INTELLIGENT_DRUM_LAB.md`.

## 7. Related Docs

- APC Constitution & Specs: `../../plugins/DrumDesignLab/DPCS_PHASE0_CONSTITUTION.md`, `../../plugins/DrumDesignLab/.ideas/`
- Repo Architecture Index: `../../ARCHITECTURE.md`
- Agent skills: `.cursor/skills/drum-design-lab/`, `genre-drum-engine/`, `intelligent-drum-lab/`, `overnight-kit-agent/`, `ddl-standalone-build/`
- Intelligent Lab: `docs/INTELLIGENT_DRUM_LAB.md`
- Overnight harness: `../../overnight-agent/ARCHITECTURE.md`
