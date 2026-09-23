# Hardware MIDI editor registry

Add one section per repo the maintainer provides. **License** must be noted; **GPL** trees stay under `hardware-lane/editors/` only.

| Device | Submodule path | Upstream | License | Status |
|--------|----------------|----------|---------|--------|
| Korg **microKORG** | `open-microkorg/` | https://github.com/gabrielmagno/Open-microKORG | GPLv3 | submodule (CLI `.prg`/`.syx`) |
| Korg **microKORG** | `alapatch/` | https://github.com/ifacodes/alapatch | (no LICENSE file upstream — verify before redistribute) | submodule (React GUI, SysEx) |
| Novation **UltraNova** | `ultranova-web/` | *(team-owned in-repo)* | MIT (editor code) | Web MIDI NRPN/CC + SysEx I/O |
| Korg **Minilogue XD** | `minilogue-xd/` | *(index only)* | — | Doc hub; see README |
| Korg **Minilogue XD** | — | [Korg Sound Librarian](https://www.korg.com/us/support/download/product/0/811/) | Korg EULA | Install on maintainer Mac/Win; libraries `.mnlgxdpreset` / `.mnlgxdunitt` |
| Korg **Minilogue XD** | — | https://github.com/isnotinvain/minilogue-xd-util | (upstream) | CLI `.mnlgxdprog` / `.mnlgxdlib` |
| Korg **Minilogue XD** | — | https://github.com/rvller/minilogue-xd-random-patch-generator | Apache-2.0 | Pure Data random CC (optional) |
| Korg **Minilogue XD** | `minilogue-xd-vibe/` *(planned)* | *(team-owned in-repo)* | MIT (planned) | JSON → Web MIDI CC/NRPN |

## Pending (paste URL → agent adds row + submodule)

- Micro-X, SE-02, MicroFreak, …
