# Night Circuit (Prophet Rev2–influenced trap / vaporwave poly synth)

**Purpose:** **Night Circuit v0.1** — polyphonic **synthetic** trap/vaporwave instrument: dual oscillators, **24 dB ladder filter** with analog-style saturation, **amp + filter ADSRs**, **unison**, **mono legato glide**, key tracking, **five factory categories + User bank**. No acoustic samples.

**Users:** Producers targeting Atlanta / Pierre / Mike Dean–style synthetic timbres (factory presets only in v0).

## Build & run

From repo root (same as JD Upgraded / WAVE-909):

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target ProphetRev2Trap_VST3 ProphetRev2Trap_Standalone
```

## Data flow

```
MIDI → Synthesiser (16 voices)
         each voice: osc1 (saw) + osc2 (pulse, detuned)
                   → SVF low-pass (cutoff + filter envelope)
                   → amp ADSR → stereo bus → output gain
APVTS ← factory presets (PresetManager categories)
UI: category ComboBox → preset ComboBox → applyFactoryPreset
```

## Threading / realtime

- Parameter reads via `getRawParameterValue` once per block in `refreshRuntimeParams`.
- Per-voice ADSR and filter; no heap alloc on audio thread.

## Key modules

| Path | Role |
|------|------|
| `Source/PluginProcessor.*` | APVTS, voices, preset apply |
| `Source/Presets/FactoryPresets.*` | Trap synthetic factory programs by category |
| `Source/Presets/PresetManager.*` | Category ↔ global preset index |
| `Source/PluginEditor.*` | Preset manager UI + envelope knobs |

## Preset categories

`Bass`, **`Synth`** (poly chords & melodic beds), `Lead`, `Pad`, `Pluck`, `Keys`, `Synth FX` — all programs use oscillators only.

Taxonomy: [docs/PRESET_CATEGORIES.md](docs/PRESET_CATEGORIES.md) · Roadmap: [docs/NIGHT_CIRCUIT_PLAN.md](docs/NIGHT_CIRCUIT_PLAN.md)

## Extension points

- Osc waveforms / second oscillator sync (Prophet-style)
- User preset save/load (JSON or host programs)
- CLAP target via `clap_juce_extensions` (root pattern)
- FX: drive, chorus, unison (post-MVP)

## Related docs

- [docs/VST_PLUGIN_FACTORY_AGENT.md](../docs/VST_PLUGIN_FACTORY_AGENT.md)
- Root [ARCHITECTURE.md](../ARCHITECTURE.md)
