---
name: sample-kit-prep
description: Normalize, name, and pack a commercial drum kit. Use when the user says sample kit, one-shots, or prep-kit.
---

# Sample kit prep

Follow `.cursor/skills/drum-kit-factory/SKILL.md` and run `process_kit.py`.

Folder layout for a sale-ready pack:

```text
KitName/
  OneShots/Kicks|Snares|Hats|Percs/
  Loops/
  Presets/
  manifest.json
  LICENSE.txt
```

Evidence of done: every one-shot peaks at about -0.3 dBFS, `getsampwidth()==3`, rate is 48000, the zip exists, and a human audition is still required before a price is shown.
