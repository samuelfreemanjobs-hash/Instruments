# DISKLORDZ SUPPLY — weekly drum factory

## Purpose

The **Supply track** manufactures **one drum kit per calendar week** (`DL-KIT-*` plus supporting `DL-DRM-*` one-shots). It runs on the **core factory orchestrator** and **does not wait** for:

- OpenClaw / vintage collective ensemble merges
- `compiled_music_prompt.json` Maestro compiles
- Research bootstrap or A&R greenlights

Records + vintage collective work **in parallel** as enrichment and catalog depth — they are **not gates** for Supply cadence.

## Cadence

| Setting | Value |
|---------|--------|
| Target | **1 kit / ISO week** |
| Mission tag | `SUPPLY-WEEKLY-DRUM` |
| Batch prefix | `DL-SUPPLY-*` (via standard batch id counter) |
| Approval | Kit enters **human approval queue** after QA stub |

## API

| Method | Path | Description |
|--------|------|-------------|
| `GET` | `/supply/weekly-drum-kit/status` | Rotation index, last run, due flag |
| `POST` | `/supply/weekly-drum-kit/run` | Run weekly job (`force=true` bypasses same-week guard) |

```bash
curl -s http://127.0.0.1:8787/supply/weekly-drum-kit/status
curl -s -X POST 'http://127.0.0.1:8787/supply/weekly-drum-kit/run?force=false'
```

## Rotation

Machine + tag rotation lives in [`disklordz-factory/database/supply_weekly_drum_rotation.json`](../disklordz-factory/database/supply_weekly_drum_rotation.json). Each run advances the index modulo list length so kits stay varied without artist-lane dependency.

## CI

[`.github/workflows/supply-weekly-drum-kit.yml`](../.github/workflows/supply-weekly-drum-kit.yml) — Monday 09:00 UTC cron hits the factory API (configure `FACTORY_API_URL` secret) or runs the Python module directly in-repo for dev.

## Related

- [BUSINESS_ENGINES.md](../disklordz-factory/docs/BUSINESS_ENGINES.md) — Supply engine
- [FACTORY_VINTAGE_COLLECTIVE.md](FACTORY_VINTAGE_COLLECTIVE.md) — parallel, non-blocking
- [disklordz-factory/ARCHITECTURE.md](../disklordz-factory/ARCHITECTURE.md)
