---
name: batch-synth-exporter
description: Renders 24-bit 48 kHz drum one-shots from the shared JUCE voices with DrumBatchExporter. Use when exporting a kit without a DAW.
---

# Batch synth exporter

```bash
cmake --build disklordz/drum-engine/build --target DrumBatchExporter
./disklordz/drum-engine/build/DrumBatchExporter \
  --out /tmp/dlz-kit --voice all --count 50 --seed 42 --brand DLZ --kit 808Lab
```

`--voice` is `kick`, `snare`, `clap`, `hat`, or `all`. The tool writes mono 24-bit 48 kHz files, removes the mean, trims the tail, fades the edges, and peak-normalizes to -0.3 dBFS.

Then stitch a preview:

```bash
python3 disklordz/drum-engine/python/stitch_preview.py /tmp/dlz-kit \
  --out /tmp/dlz-kit/preview.wav --bpm 140 --seconds 30 --mp3
```

A Python sketch of a new algorithm belongs in `disklordz/drum-engine/python/` and must pass `visualize_wav.py` before the math moves into a voice header.
