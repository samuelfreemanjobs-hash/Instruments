---
name: drum-kit-factory
description: Masters a folder of drum bounces to 24-bit 48 kHz, trims silence at a zero crossing, normalizes peaks, and zips the kit. Use for /prep-kit, sample-kit-prep, or a folder of raw one-shots.
---

# Drum kit factory

Run this. Do not hand-roll a second normalizer.

```bash
python3 disklordz/drum-engine/python/process_kit.py RAW_DIR \
  --out disklordz/drum-engine/export/KIT --brand DLZ --kit 808Lab \
  --rate 48000 --peak-db -0.3 --zip
```

What the script does:

1. Trim leading silence and back up to a zero crossing so the transient stays.
2. Peak-normalize one-shots to -0.3 dBFS. Names containing `loop` use -1.0 dBFS.
3. Optional `--saturate` applies `tanh` then normalizes again.
4. Write mono 24-bit 48 kHz WAV with no source metadata chunks.
5. Rename to `Brand_Kit_Type_01_Note.wav`.
6. Write `manifest.json` and, with `--zip`, a zip of that folder.

Then audit:

```bash
python3 disklordz/drum-engine/python/visualize_wav.py EXPORT/DLZ_808Lab_808_01_G1.wav
python3 disklordz/sound-factory/scripts/kit_qa.py KIT_DIR   # only for 44.1 kHz / 16-bit catalog packs
```

Do not point `process_kit.py` at `disklordz/sound-factory/kits/memphis-vault-vol-1` and replace that pack. That SKU is 44.1 kHz / 16-bit.

Tests: `python3 disklordz/drum-engine/python/test_workflows.py`.
