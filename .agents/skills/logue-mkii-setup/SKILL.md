---
name: logue-mkii-setup
description: Bootstrap logue-sdk and ARM gcc for mkII builds (/logue-mkii-setup).
---

# Setup (`/logue-mkii-setup`)

1. `chmod +x tools/mkii-automate.sh tools/osc-eval-mkii.sh tools/build-mkii.sh`
2. `./tools/mkii-automate.sh bootstrap` — clones `.deps/logue-sdk`, installs gcc 10.3 per SDK script.
3. Export for session: `export LOGUE_SDK="${PWD}/.deps/logue-sdk"` (or user path).
4. Smoke: `./tools/osc-eval-mkii.sh tr808_kick_phonk --static-only`

If bootstrap fails (network), document manual clone + `LOGUE_SDK` in `.agent/setup-notes.md` for the active slug.

Do not edit oscillator DSP in this phase.
