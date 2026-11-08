---
name: VST Preset Agent learnings
description: Lineage DB + daily batch runbook
type: feedback
---

- **Lineage:** `data/vst-preset-lineage/presets.sqlite` — never ship duplicate `(product, name)` or `(product, param_hash)`.
- **Daily:** `workflow-automation` cron `vst-preset-daily-batch.yml`; agent runs `daily_preset_batch.py` (WAVE + Plugg only).
- **JD trap bass:** every 11th variant in bass category uses `Trap Bass` stack in `FactoryPatchLibrary.cpp`.
