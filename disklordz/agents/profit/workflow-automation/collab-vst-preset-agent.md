# Handshake — Workflow Automation × VST Preset Agent

Registered **daily autonomous preset batch** for in-plugin synths:

- Workflow: `.github/workflows/vst-preset-daily-batch.yml` (`cron: 0 14 * * *` UTC)
- PR path: `.github/workflows/vst-preset-factory.yml` on preset DSP edits

SP-1200 is **out of scope** for daily batches (sampler maintenance only).

When editing fleet automation, keep `vst-preset-agent` `automate` string in `_specs.json` aligned with both workflow files.
