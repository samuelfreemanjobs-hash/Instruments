# VST preset lineage (VST Preset Agent)

SQLite database **`presets.sqlite`** records every factory preset name and parameter fingerprint so daily batches and agents do not ship duplicates.

- **Active seeds JSON:** `active_seeds.json` — promoted presets codegen’d into `ActiveAgentSeeds.cpp`
- **Ingest:** `python3 scripts/vst/ingest_vst_lineage.py` (hand-curated names from source)
- **Daily batch:** `python3 scripts/vst/daily_preset_batch.py` (WAVE-9090 + Plugg VA; SP-1200 excluded)

Shared pattern with SynthForge `synth_memory.db` (`hardware-preset-designer`).
