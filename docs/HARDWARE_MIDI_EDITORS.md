# Hardware MIDI editors (maintainer lane)

Physical synth editors and librarians live under **`hardware-lane/`**, separate from JD Upgraded SysEx ([SYSEX.md](SYSEX.md)) and Gearmulator ([GEARMULATOR_LANE.md](GEARMULATOR_LANE.md)).

**Rack inventory:** [HARDWARE_CAPTURE_RACK.md](HARDWARE_CAPTURE_RACK.md)

---

## Open-microKORG (microKORG)

| | |
|--|--|
| **Upstream** | https://github.com/gabrielmagno/Open-microKORG |
| **License** | GPLv3 |
| **Submodule** | `hardware-lane/editors/open-microkorg` |
| **Does** | Decode/encode **`.prg`** and **`.syx`**; text representation of programs |

```bash
git submodule update --init hardware-lane/editors/open-microkorg
pip install -r hardware-lane/requirements-editors.txt
./hardware-lane/scripts/microkorg-decode.sh /path/to/your_patch.prg
```

**Do not commit** factory sound banks or bulk `.syx` dumps unless you have redistribution rights. Use `hardware-lane/manifests/` (gitignored personal index) for local paths.

**Capture workflow:** edit/save patch on MicroKorg → note program name → hardware MIDI+audio grid → `instrument.map.json` → MPC Live. Optional: store `preset_path` in capture-plan as path to `.prg`/`.syx`.

Rust reimplementations live upstream under `alternatives/rust*` — optional future native tools.

---

## Alapatch (microKORG GUI)

| | |
|--|--|
| **Upstream** | https://github.com/ifacodes/alapatch |
| **Submodule** | `hardware-lane/editors/alapatch` |
| **Does** | **React** patch editor — dual timbre UI, drag-and-drop **SysEx** files (`.syx`); live USB MIDI planned v2.0 |

```bash
git submodule update --init hardware-lane/editors/alapatch
./hardware-lane/scripts/alapatch-dev.sh
# Browser → http://localhost:3000 — design patch, export SysEx, load on hardware when file I/O is complete
```

Pair with **Open-microKORG** for CLI decode of the same SysEx or `.prg` banks. Korg MIDI spec: [microKORG MIDI implementation PDF](http://i.korg.com/uploads/Support/MK1_633652915168960000.pdf) (linked upstream).

**Note:** Upstream README lists file save/load and Web MIDI as incomplete; confirm in UI before relying on round-trip.

**Node:** CRA 4 / craco may need **Node 16–18** (`nvm use 18`) if `npm run build` fails on Node 22 with PostCSS export errors.

---

## Adding the next repo

1. Tell the agent the GitHub URL (you’re collecting these).  
2. Agent adds submodule under `hardware-lane/editors/<name>/`, row in [REGISTRY.md](../hardware-lane/editors/REGISTRY.md), and a section here.  
3. Update [.cursor/agent-memory/team-context.yaml](../.cursor/agent-memory/team-context.yaml) `hardware_editors` list.

---

## Related

- [hardware-lane/ARCHITECTURE.md](../hardware-lane/ARCHITECTURE.md)
- [MULTISAMPLE_TO_INSTRUMENT_WORKFLOW.md](MULTISAMPLE_TO_INSTRUMENT_WORKFLOW.md)
