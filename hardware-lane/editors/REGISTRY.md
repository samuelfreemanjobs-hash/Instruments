# Hardware MIDI editor registry

Add one section per repo the maintainer provides. **License** must be noted; **GPL** trees stay under `hardware-lane/editors/` only.

| Device | Submodule path | Upstream | License | Status |
|--------|----------------|----------|---------|--------|
| Korg **microKORG** | `open-microkorg/` | https://github.com/gabrielmagno/Open-microKORG | GPLv3 | submodule (CLI `.prg`/`.syx`) |
| Korg **microKORG** | `alapatch/` | https://github.com/ifacodes/alapatch | (no LICENSE file upstream — verify before redistribute) | submodule (React GUI, SysEx) |
| Novation **UltraNova** | `ultranova-web/` | *(team-owned in-repo)* | MIT (editor code) | Web MIDI NRPN/CC + SysEx I/O |

## Pending (paste URL → agent adds row + submodule)

- Micro-X, Minilogue XD, SE-02, MicroFreak, …
