---
name: preset-factory
description: Generates factory presets inside APVTS ranges and an SFZ keymap from WAV filenames. Use for preset banks, round-robins, or velocity layers.
---

# Preset factory

```bash
python3 disklordz/drum-engine/python/preset_factory.py \
  --params disklordz/drum-engine/params.json \
  --out /tmp/dlz-presets \
  --sfz-from /tmp/dlz-kit \
  --sfz-out /tmp/dlz-kit/kit.sfz
```

- Every value stays inside `params.json` min/max.
- Categories already seeded: Heavy Sub 808s, Crispy Trap Snares, Lo-Fi Dusty Hats, Open Hat Wash, Tight Claps.
- Filename tokens: `_v1` is velocity 1–63, `_v2` is 64–127, `_rrN` sets `seq_position`, `_G1` / `_C1` / `_D1` / `_F#1` map to GM notes via `MidiMaps.h`.

Hand the bank to `preset-designer` when it will be more than a handful of files, so the main thread does not hold the JSON.
