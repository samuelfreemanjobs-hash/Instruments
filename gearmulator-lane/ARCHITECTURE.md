# Gearmulator reference lane (Instruments monorepo)

## Purpose

Hold **optional, out-of-tree** builds of [Gearmulator](https://github.com/dsp56300/gearmulator) — low-level IC emulation of late-90s/2000s VA synths and effects (VST3, CLAP, AU, LV2). This lane is for **research, A/B listening, and SysEx/MIDI behavior reference**. It is **not** part of root CMake and does **not** ship with JD Upgraded retail builds.

Relevant emulator for Roland work in this repo: **JE-8086** (Roland JP-8000). JD Upgraded targets **JD-800 / JD-990** ergonomics with a **clean-room ROM** — a different product and legal model (see [docs/GEARMULATOR_LANE.md](../docs/GEARMULATOR_LANE.md)).

## Build & run

Not built by root `cmake`. On **Linux** (Cloud Agent / CI-style smoke):

```bash
./scripts/gearmulator/build-je8086-console.sh
```

**Docker (host with Docker Engine):** [docker/Dockerfile](docker/Dockerfile) — reproducible multisample batch; mount plugins + ROM + output volumes. See [docs/GEARMULATOR_MULTISAMPLING.md](../docs/GEARMULATOR_MULTISAMPLING.md).

Get the submodule copy:

```bash
git submodule update --init --recursive gearmulator-lane/gearmulator
```

Full plugin build:

```bash
cd gearmulator-lane/gearmulator
./linux_dependencies.sh   # once per machine
./build_linux.sh
```

Upstream docs: upstream `README.md`, `CLAUDE.md`, `doc/adding_a_product.md`.

**ROM / firmware:** Gearmulator runs **original device firmware**. You must supply legally obtained ROMs (often as `.mid` SysEx dumps or `.bin` images). **Do not commit** Roland or third-party dumps to this monorepo. See [docs/INSTALLER_POLICY.md](../docs/INSTALLER_POLICY.md) and [docs/USER_ROM.md](../docs/USER_ROM.md) for JD Upgraded policy.

**License:** Gearmulator is **GPLv3**. Linking or combining its sources with proprietary JD Upgraded code in one binary requires a deliberate license strategy; keep trees separate unless legal review approves integration.

## Data flow

```
User-owned JP-8000 ROM files (.mid / .bin)
    → jeLib RomLoader (Gearmulator)
    → emulated MC68K + custom chips + firmware
    → JE8086TestConsole or JE-8086 VST3/CLAP plugin
```

JD Upgraded (in-repo) path:

```
GenerateCleanroomRom → jdupg_cleanroom.rom → RomBank → JUCE plugin
```

## Threading / realtime

Gearmulator plugins follow JUCE realtime rules; emulation threads and DSP56300 JIT are upstream concerns. Agent work here is **offline** (clone, configure, console smoke tests).

## Key modules (upstream)

| Upstream path | Responsibility |
|---------------|----------------|
| `source/cpu/dsp56300` | DSP56300 core |
| `source/cpu/mc68k` | 68K for Virus / JP-class firmware |
| `source/ronaldo/je8086/jeLib` | JP-8000 device, ROM loader, patches |
| `source/ronaldo/je8086/jeTestConsole` | Headless `JE8086TestConsole` |
| `source/framework/synthLib` | Device abstraction shared across emulators |

## Extension points

- Add a new upstream synth: use Gearmulator CMake flags (`gearmulator_SYNTH_*`) in a dedicated script under `scripts/gearmulator/`.
- Promote a SKU: open a **Planner + Marketing** row and a JUCE factory WO — do not ship GPLv3 emulation inside JD Upgraded without explicit approval.
- Compare SysEx: use JE-8086 behavior as reference only; JD import remains [docs/SYSEX.md](../docs/SYSEX.md).

## Related docs

- [docs/GEARMULATOR_LANE.md](../docs/GEARMULATOR_LANE.md) — lane policy vs JD Upgraded
- [docs/DISKLORDZ_PLUGIN_TRACKS.md](../docs/DISKLORDZ_PLUGIN_TRACKS.md) — Track **E**
- [docs/ARCHITECTURE.md](../docs/ARCHITECTURE.md) — JD Upgraded product architecture
