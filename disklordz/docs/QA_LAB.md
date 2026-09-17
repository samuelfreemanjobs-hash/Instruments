# Audio QA Lab

LLMs do not replace **measurable** plugin and audio validation. QA Lab is a subsystem with defined profiles, not a single “QA agent” chat.

## Profiles

### `plugin_technical`

| Check | Tool / method |
|-------|----------------|
| Loads / unloads | pluginval, VST3 validator |
| State save/restore | automated preset cycle test |
| Parameter automation | stepped automation sweep |
| MIDI | note + CC smoke |
| Preset load | all factory presets |
| Crashes / leaks | CI + repeated load loop |
| CPU | benchmark block at 48k/512 |
| Latency | reported latency vs spec |
| Denormals | silence input test |
| Sample rates | 44.1, 48, 96 kHz |
| Block sizes | 64–1024 |

### `plugin_audio`

| Check | Method |
|-------|--------|
| Silence in → no unexpected energy | RMS threshold |
| DC offset | high-pass meter |
| Runaway / feedback | bounded output test |
| Clipping | only when effect intends |
| Stereo / mono | correlation tests |
| Bypass | null diff within tolerance |
| Automation continuity | no clicks on ramp |

### `sound_asset`

| Check | Method |
|-------|--------|
| Clipping | peak detect |
| DC | offset correction flag |
| Duration / format | ffprobe |
| Duplicate | perceptual hash |
| DNA completeness | schema validate |

## DAW compatibility matrix

Maintained in Airtable **DAW Matrix** (manual + automated smoke where headless allows).

```text
                 WINDOWS
                    │
    ┌───────────────┼───────────────┐
    ▼               ▼               ▼
 Ableton        FL Studio        Reaper
    │               │               │
    ▼               ▼               ▼
  VST3            VST3            VST3

macOS (later): Apple Silicon / Intel — AU + VST3
```

Each cell: `last_tested`, `version`, `pass/fail`, `notes`, link to WO.

## CI integration

- Existing: `.github/workflows/build-plugin.yml` + pluginval  
- Add: `qa-lab/profiles/plugin_technical.yaml` consumed by CI  
- Release gate: no `Released` without QA profile pass recorded on `Releases`

## QA “agent” role

The **QA Engineer** team role runs profiles, uploads logs/artifacts, updates Airtable—does not “eyeball” code only.
