# DisklordzSynth — commercial / product notes

## Ownership

All audio produced by **DisklordzSynth** tools is **generated from in-repo algorithms** (no sampled third-party content). The repository author intends these engines and their outputs to be **productized separately** from the rompler VSTi.

## What you can sell (typical)

- WAV one-shots and multis exported via `DisklordzSynth_RenderWav` / `DisklordzSynth_ExportWavPack`
- Custom `.dlrom` packs built with `DisklordzSynth_BuildFactoryPack` or extended builders
- Future standalone plugin wrappers around individual engines (bell/keys/pad/sub)

## What stays in-repo

- Source for engines (`src/ProceduralSynth.cpp`, etc.)
- CLI binaries (build targets listed in `ARCHITECTURE.md`)
- Tests (`DisklordzSynthTests`)

## Licensing

Follow the repository root **LICENSE** for code. Set your own **sample pack EULA** when selling exported WAVs/packs to customers.

## Extending

Add engines in `ProceduralSynth.*`, expose via `EngineId` and `DisklordzSynth_RenderWav`. Register new factory maps in `FactoryPackBuilder.cpp` or add alternate builders (e.g. `buildPhonkPackV2`).
