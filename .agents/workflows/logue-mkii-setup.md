---
description: logue mkII — bootstrap LOGUE_SDK and ARM toolchain
---

# /logue-mkii-setup

**Skill:** `.agents/skills/logue-mkii-setup/SKILL.md`

1. Run `chmod +x tools/mkii-automate.sh tools/osc-eval-mkii.sh`
2. Run `./tools/mkii-automate.sh bootstrap` (or document manual `LOGUE_SDK` + gcc).
3. Verify `arm-none-eabi-gcc --version` and `test -d "${LOGUE_SDK:-.deps/logue-sdk}"`.
4. Optional: `./tools/osc-eval-mkii.sh tr808_kick_phonk --static-only`

**Stop.** Tell user: `Next: /logue-mkii-brief <slug>` or `/logue-mkii-new <slug>`
