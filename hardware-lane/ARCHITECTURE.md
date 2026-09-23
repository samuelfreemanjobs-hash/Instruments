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

Minilogue XD: Korg Sound Librarian (.mnlgxdpreset / libraries) + optional minilogue-xd-util;
live design via CC/NRPN (rvller PD or planned minilogue-xd-vibe) → Write on hardware → capture.
```

## Key modules

| Path | Responsibility |
|------|----------------|
| `editors/open-microkorg/` | Submodule: CLI decode/encode `.prg`/`.syx` |
| `editors/alapatch/` | Submodule: [ifacodes/alapatch](https://github.com/ifacodes/alapatch) React GUI (SysEx) |
| `editors/ultranova-web/` | Team-owned Vite/React **UltraNova** Web MIDI editor |
| `editors/minilogue-xd/` | **Minilogue XD** doc index (Korg Librarian, util, rvller; vibe editor planned) |
| `editors/REGISTRY.md` | Index of editor repos (add more as maintainer provides URLs) |
| `scripts/microkorg-decode.sh` | Thin CLI wrapper |
| `scripts/ultranova-web-dev.sh` | Dev server for UltraNova web editor |
| `manifests/` | Optional patch index JSON (paths local only) |

## Extension points

- Add submodule + row in `REGISTRY.md` + note in `docs/HARDWARE_MIDI_EDITORS.md`.
- Wire capture-plan TSV: `preset_path` → decoded program name for `instrument_label`.

## Related docs

- [docs/HARDWARE_MIDI_EDITORS.md](../docs/HARDWARE_MIDI_EDITORS.md)
- [docs/HARDWARE_CAPTURE_RACK.md](../docs/HARDWARE_CAPTURE_RACK.md)
- [docs/TEAM_AGENT_MEMORY.md](../docs/TEAM_AGENT_MEMORY.md)
