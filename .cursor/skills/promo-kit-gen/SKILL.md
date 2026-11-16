---
name: promo-kit-gen
description: Builds a 140 BPM preview and storefront copy from a kit folder and the parameter list. Use for demos, product pages, or launch emails.
---

# Promo kit

```bash
python3 disklordz/drum-engine/python/stitch_preview.py KIT_DIR \
  --out /tmp/preview.wav --bpm 140 --seconds 30 --mp3
```

The pattern is kick, hat, snare, hat. The mix is peak-normalized to -1.0 dBFS. ffmpeg writes the mp3 when libmp3lame is present; otherwise the WAV remains.

Storefront copy:

- Read `disklordz/website/ARCHITECTURE.md` and `params.json` or the kit `manifest.json`.
- State rate, bit depth, format list, and macOS/Windows only when those artifacts exist.
- Do not invent a price, a review, a founding year, or "Apple Silicon native" unless the build is a universal binary.
- Tone is producer-to-producer. No "game-changer" or "revolutionary".
- Draft a product paragraph, three short demo beats, and one email. Do not send the email.
