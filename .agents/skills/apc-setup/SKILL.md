---
name: apc-setup
description: Noizefield-style APC first-run setup for Instruments repo. Trigger /apc-setup.
---

# APC Setup

1. If no `apc.config.json`, copy from `apc.config.example.json`.
2. Run `bash scripts/system-check.sh --human`; report failures with install links.
3. `mkdir -p plugins release reference-audio`
4. `git submodule update --init vst/JUCE` (from repo root).
5. Optional: `bash tools/mkii-automate.sh bootstrap` only if user also targets NTS-1 mkII.
6. Set `setup.completed` true and `setup.platform` via `bash bin/apc setup`.

**Do not** create plugin files. Stop with: `Next: /apc-dream <Name>`
