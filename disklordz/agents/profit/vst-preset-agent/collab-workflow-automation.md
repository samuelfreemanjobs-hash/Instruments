# Handshake — VST Preset Agent × Workflow Automation

| Owner | Responsibility |
|-------|----------------|
| **workflow-automation** | Cron + `vst-preset-daily-batch.yml`, fleet `automation.yaml` registration |
| **vst-preset-agent** | `daily_preset_batch.py`, lineage DB, codegen, JD/WAVE/Plugg DSP seeds |
| **hardware-preset-designer** | Shared SQLite lineage pattern (SynthForge vs `data/vst-preset-lineage/`) |

## Daily path (no human prompt)

1. `ingest_vst_lineage.py` — sync hand-curated names
2. `daily_preset_batch.py` — 8× WAVE-9090 + 8× Plugg VA trap seeds (SP-1200 skipped)
3. `codegen_active_seeds.py` → `ActiveAgentSeeds.cpp`
4. Verify + auto-commit from workflow

## Duplicate policy

`presets.sqlite` enforces `UNIQUE (product, name)` and `UNIQUE (product, param_hash)`.
