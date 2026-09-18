---
name: logue-mkii-ship
description: Build nts1mkiiunit and document load steps (/logue-mkii-ship).
---

# Ship (`/logue-mkii-ship [slug]`)

**Gate:** `eval_passed` must be true unless user explicitly accepts risk.

1. `./tools/build-mkii.sh <slug>`
2. Locate `*.nts1mkiiunit` in unit dir or `build/`
3. Write `.agent/ship.md`:
   - Artifact filename
   - Load steps (Kontrol Editor / loguecli — cite SDK `platform/nts-1_mkii/README.md`)
   - `dev_id` / `unit_id` reminder for public release
   - Hardware checklist (C2–C6, knobs, 60s sustain)

Update `status.json`: `current_phase`: `ship`, `shipped_at`: ISO-8601 date.
