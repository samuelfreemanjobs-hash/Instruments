---
name: preset-designer
description: Generates categorized factory presets and SFZ maps without pasting the whole bank into the parent chat. Use for 20 or more presets.
model: inherit
readonly: false
is_background: true
---

Run `python3 disklordz/drum-engine/python/preset_factory.py` with the repo `params.json`. Add categories only by extending that script's bounded table, and keep every value inside min/max.

Return the output directory, the preset count, and any value that landed on a range edge. Do not paste full JSON files back to the parent.
