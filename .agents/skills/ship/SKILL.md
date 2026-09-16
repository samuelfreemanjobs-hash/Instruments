---
name: apc-ship
description: APC Ship — copy VST3 to release/ folder.
---

# Ship phase (`/apc-ship [Name]`)

1. Locate artefact under `vst/<Name>/build/` (`*_artefacts/**/VST3/*.vst3`).
2. `mkdir -p release/<Name>`
3. Copy bundle or zip to `release/<Name>/`
4. `validation.ship_ready` = true

Document load path for user's DAW in `plugins/<Name>/README.md` if missing.
