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

## UltraNova Web Editor (Novation UltraNova)

| | |
|--|--|
| **Path** | `hardware-lane/editors/ultranova-web` |
| **License** | MIT (this repo) |
| **Does** | **Browser** patch editor — Web MIDI **NRPN/CC** live edit, SysEx **`.syx`** import/send, program / edit-buffer **dump requests** |

```bash
./hardware-lane/scripts/ultranova-web-dev.sh
# Chrome/Edge → http://127.0.0.1:5174 — connect UltraNova USB, allow MIDI + SysEx
```

**Synth setup:** SysEx receive on, write protect off (same as Novation Librarian). **Phase 1** covers real-time parameters; full **program binary** edit (every panel field from a `.syx` dump) is planned — use Novation Editor/Librarian for bank backup until then.

**Capture workflow:** design/tweak patch in browser → save on hardware → hardware audio grid → `instrument.map.json` → MPC Live.

MIDI reference: [UltraNova MIDI implementation PDF](https://downloads.novationmusic.com/novation/synthesisers/ultranova).

---

## Minilogue XD (Korg)

Maintainer index: [hardware-lane/editors/minilogue-xd/README.md](../hardware-lane/editors/minilogue-xd/README.md).

### Korg Sound Librarian (official — install only, not in git)

| | |
|--|--|
| **Upstream** | [minilogue xd downloads](https://www.korg.com/us/support/download/product/0/811/) → **Sound Librarian** 1.0.5 (Win/Mac) |
| **Does** | Reorder/backup **programs**, import **`.mnlgxdpreset`** sound packs, load **user oscillators/effects** (`.mnlgxdunitt`), **microtuning** over USB MIDI |
| **Does not** | Full live “every knob” patch design (use CC/NRPN tools below for sound design, then **Write** on hardware and manage files here) |

Install **KORG USB-MIDI Driver** from the same product page. Operation manual ships inside the Librarian app.

**Capture workflow:** design patch → **Write** on XD → backup library or single program via Librarian → note program name (and optional local `.mnlgxdlib` / `.mnlgxdprog` path in capture-plan) → audio grid → `instrument.map.json` → MPC Live.

### minilogue-xd-util (CLI libraries)

| | |
|--|--|
| **Upstream** | https://github.com/isnotinvain/minilogue-xd-util |
| **Does** | Pretty-print and manipulate **`.mnlgxdprog`** / **`.mnlgxdlib`** (Python); remap user OSC/FX slots |

Use for scripted library surgery or agent-readable patch dumps — not for live MIDI performance.

### rvller random patch generator (optional explore)

| | |
|--|--|
| **Upstream** | https://github.com/rvller/minilogue-xd-random-patch-generator |
| **License** | Apache 2.0 |
| **Does** | **Pure Data** — **TAB** sends random **MIDI CC** (toggle LFO/multi/portamento sections) |

Default **MIDI channel 5**; set **MIDI Rx CC = On** on the synth. Reference for CC numbers when building the vibe editor; not a substitute for Korg Librarian or directed presets.

### minilogue-xd-vibe (planned, in-repo)

| | |
|--|--|
| **Path** | `hardware-lane/editors/minilogue-xd-vibe/` *(future)* |
| **Goal** | Browser or Python **Web MIDI** editor: **JSON preset → CC/NRPN** (Korg **CC63 + CC6** order for NRPN data on XD), optional archetypes (“pluck”, “bass”) — same lane as [UltraNova Web Editor](#ultranova-web-editor-novation-ultranova) and **madmidi**-style capture metadata |

**MIDI reference:** [MIDI Implementation](https://www.korg.com/us/support/download/product/0/811/) (TXT on Korg downloads). SysEx program dumps: function `40` / `4C` per Korg spec (see also [minilogue XD JS editor tutorial](https://cycling74.com/tutorials/building-a-synthesizer-editor-with-javascript-part-3)).

---

## Adding the next repo

1. Tell the agent the GitHub URL (you’re collecting these).  
2. Agent adds submodule under `hardware-lane/editors/<name>/`, row in [REGISTRY.md](../hardware-lane/editors/REGISTRY.md), and a section here.  
3. Update [.cursor/agent-memory/team-context.yaml](../.cursor/agent-memory/team-context.yaml) `hardware_editors` list.

---

## Related

- [hardware-lane/ARCHITECTURE.md](../hardware-lane/ARCHITECTURE.md)
- [MULTISAMPLE_TO_INSTRUMENT_WORKFLOW.md](MULTISAMPLE_TO_INSTRUMENT_WORKFLOW.md)
