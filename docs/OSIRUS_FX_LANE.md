# Osirus FX lane (always-on bus)

## What you get

**Osirus FX** is Gearmulator’s **separate Access Virus effect plugin** (`OsirusFX` VST3, 4CC `TusF`)—not the Osirus **synth**. It runs the Virus **multi-effect DSP** (filters, delays, phaser, reverb, etc.) on **audio input**, the same family as using a Virus as an insert in a DAW.

| Use | Result |
|-----|--------|
| **Insert on captures** | Darker/brighter multisamples, “luxury” space, trap-friendly saturation/filtering **after** the source synth (JE-8086, WAVE-909, SC-55 render, etc.) |
| **Always-on policy** | Set `INSTRUMENTS_ALWAYS_OSIRUS_FX=1` so DawDreamer capture **defaults** to synth → Osirus FX graph |
| **Offline WAV** | `render_wav_osirus_fx.py` or `apply_osirus_fx_to_session.py` for dry grids |

You **do not** get a second synth engine—you get **one consistent FX signature** across all captured zones. Timbre still comes from the **source plugin + ROM**; FX shapes tone and space.

**Requirements:** Build FX VST3; Virus **ROM** may be required for Osirus FX to boot (same as synth). Never commit ROM to git.

## Build

```bash
./scripts/gearmulator/build-gearmulator-osirus-fx.sh
source gearmulator-lane/.gearmulator-osirus-fx.env
```

Writes `GEARMULATOR_OSIRUS_FX_VST3` and enables `INSTRUMENTS_ALWAYS_OSIRUS_FX=1`.

## Capture (DawDreamer)

```bash
source gearmulator-lane/.gearmulator-osirus-fx.env
python3 scripts/gearmulator/render_multisample_dawdreamer.py \
  --plugin "${GEARMULATOR_JE8086_VST3}" \
  --fx-preset /path/to/trap_bus.vstpreset \
  --name je8086_trap_bus --note-start 60 --note-end 60
```

Disable for one run: `--no-fx`.

`instrument.map.json` records `post_fx` when FX was used ([MULTISAMPLE_MAP_FORMAT.md](MULTISAMPLE_MAP_FORMAT.md)).

## mpc-agent

```bash
python3 mpc-agent/scripts/capture_and_export.py \
  --plugin build/Wave909/.../WAVE-909.vst3 \
  --source-plugin WAVE-909 --name trap909_fx
```

Honours the same env vars when mpc-agent calls the render script.

## Related

- [MPC_AGENT_DAWDREAMER.md](MPC_AGENT_DAWDREAMER.md)
- [TEAM_AGENT_MEMORY.md](TEAM_AGENT_MEMORY.md)
