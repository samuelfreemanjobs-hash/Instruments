# Gearmulator reference lane (Track E)

**Status:** Parallel R&D — **not** the ship stack for JD Upgraded unless explicitly promoted.  
**Upstream:** https://github.com/dsp56300/gearmulator  
**Monorepo anchor:** [gearmulator-lane/ARCHITECTURE.md](../gearmulator-lane/ARCHITECTURE.md)  
**Fork (recommended for multisample / custom tooling):** [GEARMULATOR_FORK.md](GEARMULATOR_FORK.md)

## Why this lane exists

Gearmulator emulates the **original microcontrollers and DSPs** of classic hardware and executes **authentic firmware ROMs**. That is useful when you need:

- Reference **MIDI / SysEx / preset** behavior on real firmware (e.g. JP-8000 via **JE-8086**).
- Listening tests against LLE plugins (Virus, Waldorf, Nord, JP-8000, MT-32 family).
- Study of how upstream structures **device libs** (`jeLib`, `synthLib`, `hardwareLib`).

**JD Upgraded** in this repo is intentionally **not** firmware LLE: it uses a **procedural clean-room ROM** and JUCE DSP inspired by JD-800/990 UX ([docs/ROM.md](ROM.md), [docs/ARCHITECTURE.md](ARCHITECTURE.md)).

| Aspect | JD Upgraded (Track C) | Gearmulator JE-8086 (Track E) |
|--------|------------------------|-------------------------------|
| Target hardware lineage | JD-800 / JD-990 | JP-8000 |
| Sound source | `jdupg_cleanroom.rom` | User-supplied JP firmware ROM |
| Engine | Native JUCE C++ | 68K + custom chips + ROM |
| License | Project default | **GPLv3** (upstream) |
| Ships in retail installer | Yes (clean-room only) | **No** — external clone |

## Agent workflow

1. Read root [ARCHITECTURE.md](../ARCHITECTURE.md) and [gearmulator-lane/ARCHITECTURE.md](../gearmulator-lane/ARCHITECTURE.md).
2. Smoke-build without ROM: `./scripts/gearmulator/build-je8086-console.sh` (compiles `JE8086TestConsole`; runtime still needs user ROM).
3. Do **not** commit ROM dumps or SysEx banks. The upstream tree lives in submodule `gearmulator-lane/gearmulator/` — update the pinned SHA with `git submodule update --remote` only when intentional.
4. Do **not** copy GPLv3 sources into `Source/` without a license WO.

## Supported devices (upstream)

Osirus / OsTIrus (Access Virus), Vavra (Waldorf microQ), Xenia (Microwave II/XT), Nodal Red 2x (Nord Lead 2x), **JE-8086** (Roland JP-8000), **88emu** (MT-32 / Sound Canvas family). Enable per target with `gearmulator_SYNTH_*` CMake options (see upstream README).

## Cloud / Linux dependencies

Beyond the default Instruments image (`g++-12`, JUCE deps), Gearmulator may need:

```bash
sudo apt-get install -y libxcomposite-dev freeglut3-dev pkg-config
```

`linux_dependencies.sh` in the upstream repo lists the full set. Submodule init is **required** (`git submodule update --init --recursive`).

## WIP rules

Track E does **not** count against the Factory Manager JUCE WIP cap ([DISKLORDZ_PLUGIN_TRACKS.md](DISKLORDZ_PLUGIN_TRACKS.md)) unless you open a port/integration WO that touches `Source/` or root CMake.
