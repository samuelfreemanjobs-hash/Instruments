# Memphis Phonk reference fingerprint (Phase 0)

Source: five analyzed reference loops (2026-09-28). Use as **QC targets** for generation and post-processing — not as training labels until one-shots are sliced and licensed.

## Per-file characterization

| File | BPM | Sub% | Centroid | RMS | Dyn range | Character |
|------|-----|------|----------|-----|-----------|-----------|
| phonk-aggressive-noisy | 140 | 74% | 2766 Hz | 0.25 | 66 dB | Sub-heavy, noisy |
| phonk-doomshop | 160 | 83% | 1742 Hz | 0.31 | 58 dB | Darkest, sub dominant |
| hard-compressed-memphis | 130 | 79% | 3459 Hz | 0.43 | 10.5 dB | Heavy limiter, dense |
| breakbeat-85 | 85 | 65% | 4516 Hz | 0.08 | 54 dB | Breakbeat, bright |
| 90s-cowbell | 140 | 32%* | 5089 Hz | 0.09 | 50 dB | Cowbell, kick-forward |

\* Cowbell shifts energy upward vs other references.

**Sub%:** share of spectral energy below ~80 Hz (808/sub lane).

## Cross-file DNA (Memphis Phonk signature)

| Dimension | Typical range | Notes |
|-----------|---------------|--------|
| Sub / 808 energy | 65–83% below 80 Hz | Cowbell reference is outlier |
| BPM | 85 · 130–140 · 160 | Breakbeat vs standard vs aggressive |
| Dynamic range | 10.5–66 dB | Smashed loops = authentic Memphis |
| Spectral centroid | 1742–3459 Hz | Exclude cowbell-heavy material |
| Onset gap (mean) | 130–218 ms | Groove density |
| Zero-crossing rate | 0.05–0.19 | Hat/noise content |
| Percussive ratio | 0.27–0.77 | Limiting reduces transients |

## How this connects to DDSP

1. **Kick/808 lane:** train [`DDSP808Encoder`](../../drum_synth_blueprint/torch_ddsp/encoder.py) on sliced kicks; spectral loss already favors sub + transient shape.
2. **Post chain:** match **Dyn range** and **centroid** per kit tier (doomshop vs compressed Memphis).
3. **Memphis Architect:** manual kick/snare design for golden references — [memphis-architect/ARCHITECTURE.md](../../../disklordz/memphis-architect/ARCHITECTURE.md).

## Next analysis artifacts (Phase 0)

- [ ] Reproducible `analyze_reference_loop.py` (librosa) checked into `tools/drum-synth-blueprint/scripts/`
- [ ] JSON export of metrics per file for SQLite ingest

Program index: [docs/products/MEMPHIS_PHONK_DRUM_KIT_ML.md](../../../docs/products/MEMPHIS_PHONK_DRUM_KIT_ML.md)
