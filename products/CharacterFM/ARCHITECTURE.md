# Character FM — architecture

**Product:** six-operator FM synthesizer (Track E) with future CHARACTER ENGINE™ layer.

## Purpose

Playable FM instrument for DAWs; Milestone 1 delivers core DSP and minimal UI. Character Engine, AI Designer, and premium UI follow later phases in [docs/CHARACTER_FM_ENGINEERING_SPEC.md](../../docs/CHARACTER_FM_ENGINEERING_SPEC.md).

## Build & run

From repo root (Release):

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build --target CharacterFM_VST3 CharacterFM_Standalone CharacterFMSmoke -j
./build/CharacterFMSmoke
```

Artefacts:

- VST3: `build/CharacterFM_artefacts/Release/VST3/Character FM.vst3`
- Standalone: `build/CharacterFM_artefacts/Release/Standalone/Character FM`

## Data flow

```text
MIDI → CharacterFMAudioProcessor → FMEngine (24 voices)
         APVTS ──► VoiceParams ──► FMVoice × N
                              ModulationGraph (32 algorithms as data)
                              6 × Operator (sine + ADSR)
         ──► stereo out (mono duplicated)
```

## Threading / realtime

- Audio thread: `FMEngine::renderBlock` only; no allocations, no locks.
- UI thread: APVTS attachments; preset JSON import/export via processor helpers.

## Key modules

| Path | Role |
|------|------|
| `Source/DSP/ModulationGraph.*` | Edge list + carrier mask |
| `Source/DSP/AlgorithmData.*` | 32 routing tables |
| `Source/DSP/Operator.*` | Sine FM operator + envelope |
| `Source/DSP/FMVoice.*` | One note, six ops |
| `Source/DSP/FMEngine.*` | Polyphony + steal |
| `Source/Parameters/*` | APVTS layout |
| `Source/Presets/PresetManager.*` | JSON preset v1 subset |

## Extension points

- Character coefficients injected at block rate into `FMVoice` (Phase 6).
- Replace/ augment `AlgorithmData` with DX7-exact graphs + morph targets.
- Plugin Kernel migration: move tree under `disklordz/plugin-kernel/products/CharacterFM/`.

## Related docs

- [CHARACTER_FM_PRODUCT_PLAN.md](../../docs/CHARACTER_FM_PRODUCT_PLAN.md)
- [CHARACTER_FM_ENGINEERING_SPEC.md](../../docs/CHARACTER_FM_ENGINEERING_SPEC.md)
