# Memphis Phonk reference loops (Phase 0)

Drop the **five characterized loops** here as flat `.wav` files (any filenames OK).

Suggested names (match [memphis_phonk_fingerprint.md](../docs/memphis_phonk_fingerprint.md)):

| Suggested filename | Character |
|--------------------|-----------|
| `phonk-aggressive-noisy.wav` | 140 BPM, sub-heavy |
| `phonk-doomshop.wav` | 160 BPM, darkest sub |
| `hard-compressed-memphis.wav` | 130 BPM, limiter-smashed |
| `breakbeat-85.wav` | 85 BPM, brighter break |
| `90s-cowbell.wav` | Cowbell-forward layer |

## Commands

```bash
cd tools/drum-synth-blueprint
pip install -r requirements-analysis.txt && pip install -e .

# Verify metrics vs fingerprint doc
python scripts/analyze_reference_loops.py --folder ./reference-loops

# Slice → training one-shots
python scripts/slice_phonk_loops.py --input ./reference-loops --output ./drums/sliced
```

**License:** commit only audio you have rights to use for training and commercial kits.

If files are large, use [Git LFS](https://git-lfs.com/) or keep loops local and pass `--folder` to the scripts.
