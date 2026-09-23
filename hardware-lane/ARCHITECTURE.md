# Hardware lane — MIDI editors & capture glue

## Purpose

Host **third-party and team-owned tools** for the maintainer’s **physical synths** ([docs/HARDWARE_CAPTURE_RACK.md](../docs/HARDWARE_CAPTURE_RACK.md)): patch decode/encode, SysEx librarians, and links to **multisample capture** (`instrument.map.json`, mpc-agent).

This lane is **not** part of root CMake JD Upgraded builds. Keep **GPLv3** editor code in submodules under `hardware-lane/editors/` — do not merge into `Source/`.

## Build & run

```bash
git submodule update --init hardware-lane/editors/open-microkorg
pip install -r hardware-lane/requirements-editors.txt
./hardware-lane/scripts/microkorg-decode.sh /path/to/patch.prg
```

Editors run **locally** (USB MIDI to hardware). Cloud Agents use this tree for **docs, scripts, and patch metadata** — not live MIDI.

## Data flow

```text
.prg / .syx (user-owned, often gitignored)
    → Open-microKORG (decode / encode / text dump)
    → optional: capture-plan row (preset name → hardware session)
    → audio grid → instrument.map.json → MPC Live
```

## Key modules

| Path | Responsibility |
|------|----------------|
| `editors/open-microkorg/` | Submodule: CLI decode/encode `.prg`/`.syx` |
| `editors/alapatch/` | Submodule: [ifacodes/alapatch](https://github.com/ifacodes/alapatch) React GUI (SysEx) |
| `editors/REGISTRY.md` | Index of editor repos (add more as maintainer provides URLs) |
| `scripts/microkorg-decode.sh` | Thin CLI wrapper |
| `manifests/` | Optional patch index JSON (paths local only) |

## Extension points

- Add submodule + row in `REGISTRY.md` + note in `docs/HARDWARE_MIDI_EDITORS.md`.
- Wire capture-plan TSV: `preset_path` → decoded program name for `instrument_label`.

## Related docs

- [docs/HARDWARE_MIDI_EDITORS.md](../docs/HARDWARE_MIDI_EDITORS.md)
- [docs/HARDWARE_CAPTURE_RACK.md](../docs/HARDWARE_CAPTURE_RACK.md)
- [docs/TEAM_AGENT_MEMORY.md](../docs/TEAM_AGENT_MEMORY.md)
