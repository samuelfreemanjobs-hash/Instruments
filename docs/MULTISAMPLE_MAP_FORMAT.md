# Multisample map format (canonical)

Every capture session **must** produce **`instrument.map.json`** alongside WAVs. That file is the **source of truth** for HISE, MPC (mpc-agent), SFZ, and future JUCE rompler importers.

Legacy **`manifest.tsv`** is a render log; tools convert it to the map automatically.

---

## Files per session

```text
gearmulator-lane/multisamples/out/<session_name>/
  instrument.map.json    ← canonical (commit template only; not WAV sessions in git)
  manifest.tsv           ← render log (same zones, tab-separated)
  note_060_vel_100.wav
  ...
  instrument.sfz         ← optional export (generated from map)
```

---

## `instrument.map.json` (version 1)

| Field | Required | Description |
|-------|----------|-------------|
| `map_format_version` | yes | Always `1` for this schema |
| `instrument_id` | yes | Stable snake_case id (SKU / mpc-agent / HISE) |
| `instrument_label` | yes | Display name for your preset/instrument |
| `source` | yes | Where samples were captured (plugin, program, preset) |
| `session` | yes | Session folder name, engine, capture date |
| `audio` | yes | `sample_rate`; optional `bit_depth` (24) |
| `zones` | yes | One object per WAV region |

### `source`

```json
{
  "plugin_path": "/absolute/or/session-relative/path/to/Plugin.vst3",
  "source_plugin": "JE-8086",
  "program": 0,
  "preset_path": null,
  "rom_note": "user_rom_not_in_git"
}
```

### `post_fx` (optional)

When capture runs **synth → Osirus FX** (see [OSIRUS_FX_LANE.md](OSIRUS_FX_LANE.md)):

```json
{
  "fx_plugin_path": "/path/to/OsirusFX.vst3",
  "fx_preset_path": "/path/to/bus.vstpreset",
  "fx_plugin": "OsirusFX",
  "always_on_policy": "INSTRUMENTS_ALWAYS_OSIRUS_FX"
}
```

### `zones[]`

```json
{
  "zone_id": "n060_v100",
  "wav": "note_060_vel_100.wav",
  "midi_note": 60,
  "root_key": 60,
  "lo_key": 60,
  "hi_key": 60,
  "lo_vel": 100,
  "hi_vel": 100,
  "seconds": 3.0
}
```

After capture, **edit in the sampler** (or in mpc-agent): widen `lo_key`/`hi_key` for crossfade zones, add `lo_vel`/`hi_vel` layers by merging multiple sessions.

---

## Policy

1. **Always run finalize** after render (automatic in DawDreamer/C++ scripts):
   ```bash
   python3 scripts/gearmulator/finalize_multisample_session.py gearmulator-lane/multisamples/out/<session>
   ```
2. **Merge velocity layers** → one `instrument_id`, multiple sessions or one merged map (mpc-agent / manual).
3. **Do not** ship only loose WAVs without a map for Disklordz / mpc-agent products.
4. **Do not** commit captured WAVs or ROM paths with secrets to public git.

---

## Exports (derived from map)

| Target | Tool |
|--------|------|
| SFZ | `python3 scripts/gearmulator/map_to_sfz.py <session>/instrument.map.json` |
| (future) HISE | mpc-agent or `hise-sketch` import reading v1 JSON |
| (future) MPC | mpc-agent |

---

## Related

- [MULTISAMPLE_TO_INSTRUMENT_WORKFLOW.md](MULTISAMPLE_TO_INSTRUMENT_WORKFLOW.md)
- [MPC_AGENT_DAWDREAMER.md](MPC_AGENT_DAWDREAMER.md)
