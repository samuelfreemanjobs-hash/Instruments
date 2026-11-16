---
name: dsp-visualizer
description: Audits a drum WAV for DC offset, crest factor, an inter-sample peak estimate, and sub-bass mono coherence. Use after DrumBatchExporter or any new voice.
---

# DSP visualizer

```bash
python3 disklordz/drum-engine/python/visualize_wav.py PATH.wav
sox PATH.wav -n spectrogram -o /tmp/dlz-spectrogram.png
```

The script fails when the mean exceeds 0.02, the 4x linear inter-sample estimate exceeds full scale, or sub-bass correlation (below 120 Hz) drops under 0.95. A quiet sub band is treated as coherent. The inter-sample figure is an estimate, not ITU-R BS.1770.

Kick and 808 names with a crest factor under 4 are a punch warning, not a failure. Hats are not expected to have sub energy.
