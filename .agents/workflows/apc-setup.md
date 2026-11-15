---
description: APC first-run — toolchain, apc.config.json, JUCE submodule
---

# /apc-setup

**Skill:** `.agents/skills/apc-setup/SKILL.md`

1. Run `bash bin/apc setup` (or wizard in skill).
2. Verify `scripts/system-check.sh --human`.
3. `git submodule update --init vst/JUCE` if missing.
4. Set `setup.completed` true in `apc.config.json`.

**Stop.** Tell user: `Next: /apc-dream <PluginName>`
