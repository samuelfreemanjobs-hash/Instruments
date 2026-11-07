# Overnight VST mega-bank jobs

Use **`AutoSamplerMegaBank`** for unattended capture of up to **26** source VST3 plugins.

## Quick start

```bash
AutoSamplerMegaBank --template ~/mega_bank.json
# edit sources[].vst3Path
AutoSamplerMegaBank ~/mega_bank.json ~/AutoSamplerExports
```

## Resume after crash

With `"run": { "resume": true, "writeProgress": true }` the job writes:

- `mega_bank_progress.json` — completed/failed sources
- `mpc_validation.txt` — per-source MPC keyzone validation
- `vst3_plugin_cache.json` — last good plugin paths

Re-run the same command; finished sources are skipped.

## Capture options (JSON `capture`)

| Field | Purpose |
|-------|---------|
| `noteMapPreset` | `chromatic`, `minorThird`, `majorThird`, `fifth`, `octave` |
| `blackKeyPolicy` | `include`, `skip`, `whiteKeysOnly` |
| `recordUntilSilence` | Extend takes through release tail |
| `multiVelocityLayers` | Four velocity passes per root (when `true`) |
| `velocityLayers` | MIDI velocities, default `[32,64,96,127]` |

## Reliability (`run`)

- `maxRetriesPerSource` — retry hung/failed plugins
- `sourceTimeoutSeconds` — per-plugin wall clock (default 3600)
- `pluginCacheFile` — optional cache JSON path

## Outputs per preset

`exportMpcKit` layout: JSON, SFZ, EXS24, MPC `.xpm` + WAVs, `README_MPC.txt`.
