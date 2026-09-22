---
name: sound-design-reference-match
description: "Match factory output to user-supplied reference audio using analysis reports and spectral diff. Use for timbre goals, 'sounds like' briefs, and QA evidence."
---

# Sound design & reference match

## Legal

- Search web for **metadata** (BPM, key, gear, era).
- Analyze **only** audio the user provided or owns/licensed.
- Do not download copyrighted tracks from streaming/YouTube without explicit user direction and rights.

## Workflow

1. Ingest reference WAV/MP3 to `disklordz/sound-factory/refs/` (or WO attachment path).
2. Run analysis (when `analyze_reference.py` exists) or interim: `ffmpeg` + Python scipy for LUFS/centroid/PNG spectrogram.
3. Derive targets: brightness, decay, transient density, stereo width → update `GenerationSpec` / factory params.
4. Regenerate batch; compare candidate vs ref slice with `tools/SpectralDiff` where applicable.
5. Attach report JSON + PNG + A/B clips to PR.

## Subagents

- `videoReview` — user reference **video** (visual + described timbre); no audio rip without permission.
- `WebSearch` — song facts, not pirated audio.

## Evidence

Spectrogram PNG, metrics JSON, and short WAV A/B are required for "matched reference" claims.
