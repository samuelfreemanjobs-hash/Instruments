---
name: intelligent-drum-lab
description: >-
  Prompt-to-kit pipeline for Drum Design Lab: parse natural language into
  PromptSpec, mutate DrumRecipe, offline render, QA, rank, auto-name. Use when
  the user asks for batch kicks/808s from a text prompt, Intelligent Lab, or
  AI-assisted but deterministic drum generation.
---

# Intelligent Drum Lab

## Authority

- `vst/DrumDesignLab/Source/Lab/IntelligentDrumLab.h`
- `vst/DrumDesignLab/docs/INTELLIGENT_DRUM_LAB.md`
- QA: `Source/Export/WavExporter.h`

## Core rule

**Deterministic C++ DSP is the source of truth.** LLMs / Stable Audio / MOSS are optional offline helpers for tagging or textures — never required for one-shots, never on the audio thread.

## Pipeline

1. `parsePrompt(text, seed)` → `PromptSpec` (count, rootHz, characters, MPC flags)
2. Oversample candidates (`count * 2`) via `applyCharacters` + `ThreeLayerVoice::renderOffline`
3. `export_wav::analyzeInterleavedStereo` — reject NaN/silent/low peak
4. Rank + near-dup filter → `RankedShot` with `DLZ_<Kind>_<Root>_NN_sNN.wav`
5. Write WAVs only from worker/offline tools — not `processBlock`

## Example prompt

> Generate 24 hard, distorted Memphis-inspired kicks, with gritty midrange, short tails, tuned to C, optimized for MPC playback.

## Extending

| Upgrade | Where |
|---------|--------|
| LLM → JSON PromptSpec | Replace keyword `parsePrompt`; keep schema |
| Generative texture | Offline only; blend into Texture layer seed |
| Audio tagging models | Post-export worker |

## Tests

Add/keep checks in `Tests/m1_core_test.cpp` for parse flags + `generateFromPrompt` keeping ≥4 ranked shots.
