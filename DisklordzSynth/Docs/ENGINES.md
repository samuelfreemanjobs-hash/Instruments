# DisklordzSynth — four engines

Each engine is a **standalone offline renderer** you can productize (CLI WAV export, custom packs, future VSTi wrappers).

| Engine | ID | Role | Rompler tone slot |
|--------|-----|------|-------------------|
| **Additive** | `additive` | Harmonic stacks, bells, metallic partials | Tone 1 (bell category) |
| **Karplus** | `karplus` | Plucks, keys, muted strings | Tone 2 (keys) |
| **Wave** | `wave` | Procedural wavetable cycles, pads/textures | Tone 3 (pad, looped) |
| **Subtractive** | `subtractive` | Saw/pulse → LP/HP-style filter, 808-style subs | Tone 4 (sub) |

## Parameters (`EngineParams`)

See [include/disklordz/EngineParams.h](../include/disklordz/EngineParams.h). CLI `RenderWav` passes **paramA** → `a`, **paramB** → `b`; other fields use defaults unless you call the library API.

## CLI

```bash
DisklordzSynth_RenderWav subtractive 12000 out.wav 0.2 0.3
DisklordzSynth_RenderWav additive 4096 bell.wav 0.6 0.4
DisklordzSynth_RenderWav karplus 8192 pluck.wav 0.99 0.35
DisklordzSynth_RenderWav wave 16384 pad.wav 0.45 0.3
```

Legacy names still work: `bell` → additive, `keys` → karplus, `pad` → wave, `sub808` → subtractive.

## Selling separately

- Ship **one engine** as a sample pack generator (document which CLI engine id).
- Ship **all four** as “Disklordz Synth Factory” bundle.
- Extend one engine in its own `.cpp` without pulling the rompler plugin.
