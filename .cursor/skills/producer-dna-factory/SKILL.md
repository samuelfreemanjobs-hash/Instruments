---
name: producer-dna-factory
description: >-
  Implements Producer DNA Drum Factory milestones on top of Drum Design Lab
  (12 producer identities, shared three-layer DSP, grooves, morph, sample
  factory). Use when the user cites Producer DNA, DJ Paul/Sonny/Boi-1da DNA,
  or the ultimate master meta-prompt for 12 engines.
---

# Producer DNA Drum Factory

## Relationship to DDL

Producer DNA is **not** a separate product root by default. Implement as:

- New portable engines under `vst/DrumDesignLab/Source/DSP/` or `Source/ProducerDNA/`
- `KitStyle` entries in `MultiGenreDrumEngine.h`
- Shared Body/Attack/Texture via `ThreeLayerDrum.h` + `PhysicalModels.h`

Follow `drum-design-lab` + `genre-drum-engine` for each identity.

## Milestone order (do not skip verification)

| M | Scope |
|---|--------|
| M1 | Shared core + kick/808/snare + offline WAV + tests + optional Standalone |
| M2 | Full 16 instrument families |
| M3–M7 | Memphis → Atlanta → Polished → Cloud/Plugg → Detroit/Flint engines |
| M8 | Genre groove modules |
| M9 | DNA morph |
| M10 | Mutation factory |
| M11 | GUI |
| M12–M13 | Commercial pack factory + QA |

**BEGIN with M1** unless user names a later milestone. One milestone per turn unless asked to continue.

## Originality

References describe sonic tradition only — no proprietary sample cloning, no copyrighted groove reproduction.
