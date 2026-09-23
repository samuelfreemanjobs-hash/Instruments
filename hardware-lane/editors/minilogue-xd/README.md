# Minilogue XD — editor & librarian index

Team docs: [docs/HARDWARE_MIDI_EDITORS.md](../../../docs/HARDWARE_MIDI_EDITORS.md) (full workflow).

| Tool | Role | Install |
|------|------|---------|
| **Korg Sound Librarian** | Official program libraries, `.mnlgxdpreset` packs, user OSC/FX (`.mnlgxdunitt`), microtuning | [Korg downloads](https://www.korg.com/us/support/download/product/0/811/) (Win/Mac v1.0.5) + [USB-MIDI driver](https://www.korg.com/us/support/download/product/0/811/) |
| **minilogue-xd-util** | CLI/library: `.mnlgxdprog`, `.mnlgxdlib` parse & remap | https://github.com/isnotinvain/minilogue-xd-util |
| **rvller PD generator** | Random **CC** patch explore (TAB); Apache 2.0 | https://github.com/rvller/minilogue-xd-random-patch-generator |
| **minilogue-xd-vibe** *(planned)* | In-repo Web MIDI **JSON preset** editor (directed design, not random) | `hardware-lane/editors/minilogue-xd-vibe/` — not started |

**MIDI spec:** [MIDI Implementation TXT](https://www.korg.com/us/support/download/product/0/811/) on Korg product downloads (Owner’s manual bundle).

**Synth:** Global → MIDI → **Rx CC On** for live CC tools; USB **MIDI channel** must match editor (rvller defaults to **5**).

**Do not commit** personal libraries or factory pack files. Index local paths under `hardware-lane/manifests/` (gitignored).
