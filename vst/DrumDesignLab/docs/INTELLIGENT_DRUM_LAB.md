# DiskLordz Intelligent Drum Laboratory

## Purpose

Combine **original deterministic C++ DSP** (Drum Design Lab engines) with **optional AI assistance** for prompting, tagging, and research textures. Generative audio models are **not** required for the core one-shot factory.

Producer example:

> Generate 24 hard, distorted Memphis-inspired kicks, with gritty midrange, short tails, tuned to C, optimized for MPC playback.

Pipeline (implemented in `Source/Lab/IntelligentDrumLab.h`):

1. Translate request → `PromptSpec` (keyword parser now; LLM→JSON later).
2. Generate candidates via ThreeLayer / Hitmaker recipes (seeded mutations).
3. Analyze peak / DC / NaN / silence (`export_wav::analyzeInterleavedStereo`).
4. Reject failures + near-duplicates.
5. Rank by measurable fit to requested character.
6. Auto-name (`DLZ_Kick_C2_01_sNN.wav`) for export.

## Research map (inspire features — do not block shipping)

| Technology | Lab use | Priority |
|------------|---------|----------|
| Stable Audio 3 | Optional offline texture / Foley / transitions | Research |
| MOSS-Audio | Drum classification, searchable tags | Research |
| Audio Flamingo Next | Compare / explain library sounds | Research |
| MiniMax-Music3 | Demo loops / promo beds — not one-shots | Low |
| TensorRT Model Connect | Future C++ inference for tagging only | Later |
| Liquid AI / small VLMs | On-device assistants | Later |

**Rule:** RT audio thread stays DSP-only. Any model inference runs offline or on a worker thread and never allocates in `processBlock`.

## Core vs optional

| Layer | Status |
|-------|--------|
| Prompt → recipe → DSP → QA → export | Core (this module) |
| LLM structured JSON from natural language | Optional upgrade |
| Generative WAV texture blend into Texture layer | Optional |
| Auto-tagging via audio-understanding models | Optional |

## Related

- `ARCHITECTURE.md` — DSP / MultiGenre / factory
- `Source/DNA/HyperDNAMorph.h` — identity morph
- Antigravity: batch pack packaging, store copy, GPU texture experiments
