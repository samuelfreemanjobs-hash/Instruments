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

## Adding the next repo

1. Tell the agent the GitHub URL (you’re collecting these).  
2. Agent adds submodule under `hardware-lane/editors/<name>/`, row in [REGISTRY.md](../hardware-lane/editors/REGISTRY.md), and a section here.  
3. Update [.cursor/agent-memory/team-context.yaml](../.cursor/agent-memory/team-context.yaml) `hardware_editors` list.

---

## Related

- [hardware-lane/ARCHITECTURE.md](../hardware-lane/ARCHITECTURE.md)
- [MULTISAMPLE_TO_INSTRUMENT_WORKFLOW.md](MULTISAMPLE_TO_INSTRUMENT_WORKFLOW.md)
