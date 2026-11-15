# VST3 for Akai MPC Sample (Melodee)

The **MPC Sample** hosts **VST3** inside **Melodee** on an embedded **ARM64 Linux** system—not a desktop x86_64 PC. Plugins from Windows, macOS, or Linux x86_64 builds will not load. This document is the contract for building and verifying plugins from this monorepo for that host.

## Requirements checklist

| Requirement | MPC Sample / Melodee |
|-------------|----------------------|
| CPU | **aarch64 (ARM64)** only — ELF in `Contents/aarch64-linux/*.so` |
| OS | **Linux** (embedded glibc) — not Windows PE or macOS Mach-O |
| Framework | **JUCE** (this repo’s `vst/` lane) or **headless** / DPF-style parameter-only UI |
| Footprint | Prefer light DSP: few voices, no huge buffers, avoid heavy analog modeling without voice limits |

### Quick verification

On any Linux machine with the `file` utility:

```bash
file path/to/YourPlugin.vst3/Contents/aarch64-linux/*
```

Each binary should report something like: **ELF 64-bit LSB shared object, ARM aarch64**.

Or use the repo helper:

```bash
./scripts/vst/verify-mpc-sample-vst3.sh path/to/YourPlugin.vst3
```

## Three ways to build your own plugin

Melodee is **headless**: it does not open a desktop plugin window. You ship **DSP + parameters** (and optional presets). Melodee maps parameters to **K1–K3** and preset stepping; put musically important controls **early** in the parameter list (cutoff, resonance, attack, release) so you are not menu-diving on the 2.4″ screen.

| Path | Best for | Output on card | Where to work |
|------|----------|----------------|---------------|
| **[Akai MPC Plugin SDK](https://github.com/inmusic-software/mpc-plugin-builder-sdk)** (MIT) | First custom plugin with **AI-assisted** C++, MPC touchscreen **JSON UI**, fastest path to hardware knobs | Folder with `plugin.json` + **arm64/arm32 `.so`** (VST2 ABI). On **MPC OS 3.10.1+** drop under `VST2 Plugins/` on USB/SD; **Melodee** can also load the same **ARM Linux** libraries (and supports VST3/LV2/CLAP in its own browser). | Clone SDK repo; `python tools/build.py plugins/YourName`; `tools/host-sim/` before hardware |
| **JUCE (or DPF) + cross-compile** | **Standard `.vst3`**, same project targets **DAW (x86_64)** + **MPC Sample (aarch64)** | `YourSynth.vst3/Contents/aarch64-linux/YourSynth.so` → `Melodee/Plugins/` | **This monorepo:** APC lifecycle + `./tools/build-vst-aarch64-linux.sh <Name>`; optional **Podman/Docker** `dockcross/linux-arm64` if you prefer containerized aarch64 builds |
| **FAUST / Heavy (Pd)** | Rapid **algorithm** experiments without hand-writing every filter coefficient | Export **C++** or VST3 sources → feed **JUCE** or SDK project → cross-compile to aarch64 | Faust IDE → C++; or Pd → [hvcc](https://github.com/Wasted-Audio/hvcc) → integrate into your chosen wrapper |

### Official MPC Plugin SDK (summary)

- **Python 3.9+**; `build.py` fetches **Zig/GCC/Docker** cross toolchains on first run.
- You maintain **`plugin.json`** (metadata, parameters) and optional **`ui.json`** (MPC pages / Q-Links). DSP is one or more **`src/*.cpp`** files implementing the SDK’s small `process()` contract.
- **No Steinberg SDK** in-tree (clean-room VST2 ABI). Includes agent skills under `.agents/skills/` for Cursor / Claude Code.
- **Safety:** plugins run **in-process** on the device—no sandbox. Test with **host-sim** first; a crash can reboot the unit (same class of risk on Melodee).

### JUCE in this repo (summary)

- Reference plugin: **`vst/MpcAnalogSynth/`** — headless 6-voice analog polysynth, factory programs, `MPC_VST3_HEADLESS=ON` by default.
- Mono ladder line: **`vst/MpcVoyager/`**, **`vst/MpcSubPhatty/`**, **`vst/MpcMessenger/`** (wavefold + Res Bass toggle/compensation, Melodee **K1–K3** = cutoff / res / resBassToggle).
- Planned classic line (APC Plan): [plugins/MPC_CLASSIC_LINE.md](../plugins/MPC_CLASSIC_LINE.md) — VA, mono bass, FM, hybrid **MpcLaSynth**, drum synth **MpcTr808909**.
- Full **VST3** bundle layout Melodee expects.
- Use **program chain first** (DSP), keep editors minimal or headless—Melodee ignores most GUI anyway.
- Cross-build docs in the sections below; verify every release with `verify-mpc-sample-vst3.sh`.

### DSP codegen (FAUST / Heavy)

- Treat exported code as a **module**: wire into JUCE `processBlock` or SDK `process()`, then apply the same RT rules (no alloc in the callback).
- Good first targets: **mono bass** (1 voice), **simple poly** (4–8 voices), **single drum voice**—not 16-voice unison.

## Embedded coding rules (all paths)

- **CPU budget:** aim for **~15–20% per instance** on MPC Sample-class silicon; avoid heavy oversampling and huge unison counts if you want multiple tracks.
- **Parameter order:** expose **filter cutoff, resonance, amp/filter envelopes** as the first parameters when Melodee maps K1–K3 in order.
- **Memory:** allocate in **prepare** / constructor; **never** `malloc`/`new` in the audio callback (hosts are not crash-isolated).
- **Melodee test:** [melodee-sample](https://github.com/keremimo/melodee-sample) emulator / screen mirror where available; otherwise x86_64 pluginval in this repo, then aarch64 verify before SD copy.

## Creating a new plugin (APC lifecycle)

Follow the normal factory path so DSP stays maintainable:

1. Dream / plan / design under `plugins/<Name>/` (`/apc-dream` … `/apc-design`).
2. Implement with `./tools/new-vst.sh <Name>`.
3. Optimize for embedded: cap voices, avoid unbounded allocation on the audio thread, keep editor complexity modest (or plan a headless build).
4. Cross-build for MPC (below) and verify with `verify-mpc-sample-vst3.sh`.
5. Package with `./tools/package-vst.sh <Name>` after copying the **aarch64** `.vst3` into the release staging folder (or extend packaging to point at `build-aarch64/`).

## Cross-build from the repo (x86_64 Linux host)

### One-time host setup

```bash
git submodule update --init vst/JUCE

sudo apt-get update
sudo apt-get install -y build-essential cmake git g++-13 gcc-13 \
  gcc-aarch64-linux-gnu g++-aarch64-linux-gnu libstdc++-13-dev-arm64-cross
```

On hosts where `c++` is `clang++`, the build script forces **g++-13** for JUCE’s host-side `juceaide` tool (override with `HOST_CXX` / `HOST_CC`).

For plugins that use JUCE’s Linux GUI (X11/GTK), also install **arm64** development libraries on the cross sysroot (Debian multiarch):

```bash
sudo dpkg --add-architecture arm64
sudo apt-get update
sudo apt-get install -y \
  libasound2-dev:arm64 libfreetype6-dev:arm64 \
  libx11-dev:arm64 libxrandr-dev:arm64 libxinerama-dev:arm64 \
  libxcursor-dev:arm64 libgtk-3-dev:arm64
```

### Build command

```bash
./tools/build-vst-aarch64-linux.sh <PluginName>
```

This uses `cmake/toolchains/linux-aarch64-gnu.cmake` and writes artifacts under `vst/<PluginName>/build-aarch64/`. JUCE places the shared library in:

`YourPlugin.vst3/Contents/aarch64-linux/YourPlugin.so`

### Install on MPC Sample (Melodee)

Melodee scans **`Melodee/Plugins/`** on the **microSD card** you boot from (see [melodee-sample](https://github.com/keremimo/melodee-sample)). Copy each plugin as a **whole `.vst3` bundle directory** (the folder named `Something.vst3`), not a loose `.so`.

**Step-by-step (card layout)**

1. Use MPC Sample firmware **1.3.0.12** or newer and boot **Melodee** from the prepared SD image (vendor/Open MPC workflow in the Melodee repo).
2. Power off, remove the microSD, and mount it on your computer (USB reader).
3. On the card volume, open or create: `Melodee/Plugins/`
4. Copy one folder per plugin, e.g. `MelodeeChip.vst3/` (must contain `Contents/aarch64-linux/*.so` for ARM64 VST3).
5. Safely eject the card, reinsert, power on, open Melodee’s **plugin browser**.
6. Compatible plugins appear normal; **wrong-architecture** bundles (Windows, macOS, or `x86_64-linux` only) show **dimmed** and cannot be loaded.
7. Pick a plugin, choose a **preset** if available, then map **K1–K3** to parameters (Melodee does not show the plugin’s full desktop GUI).

**Before you copy a build you made yourself**, verify on the PC:

```bash
./scripts/vst/verify-mpc-sample-vst3.sh path/to/YourPlugin.vst3
```

Melodee also accepts **aarch64 LV2/CLAP**, **Open MPC** plugin folders, and **VST2** `.so` (aarch64 or **armv7** via Melodee’s bridge). This repo’s JUCE lane targets **VST3** + `Contents/aarch64-linux/`.

## Official Melodee synths (pre-built ARM64 Linux VST3)

Keremimo ships three lightweight synths built for MPC Sample; release zips use the correct bundle layout (`*-mpc-sample.zip`):

| Plugin | Role | Repo |
|--------|------|------|
| **Melodee Chip** | Chiptune / retro | [mpc-vst-melodee-chip](https://github.com/keremimo/mpc-vst-melodee-chip) |
| **Melodee Poly** | Virtual-analog poly | [mpc-vst-melodee-poly](https://github.com/keremimo/mpc-vst-melodee-poly) |
| **Melodee PD** | Phase distortion | [mpc-vst-melodee-pd](https://github.com/keremimo/mpc-vst-melodee-pd) |

Download the **GitHub release** asset, unzip, and drop the `.vst3` folder into `Melodee/Plugins/`.

## Third-party plugins (same architecture)

Any synth or effect with a real **`Contents/aarch64-linux/`** VST3 (or **armhf** VST2/armv7 build for the bridge) can work if CPU/RAM allow. Examples often seen in the Linux **arm64** plugin ecosystem (always confirm `file` / folder layout before copying):

- **Synths:** Surge XT, Dexed, Odin 2, Helm, ZynAddSubFX; some **u-he** Linux ARM builds (e.g. TyrellN6, Podolski) when published for arm64.
- **Effects:** Dragonfly Reverb, Airwindows Consolidated, TAL-Reverb / TAL-Filter, **LSP** (Linux Studio Plugins).
- **Open MPC / FM-1:** Community **arm64** ports (e.g. Open MPC release zips, The Firmware Hub) — same CPU class as Melodee; prefer ports documented for **MPC Sample**, not desktop x86_64-only drops.

**You cannot** copy a standard **Windows or macOS** `.vst3` from your DAW folder; Melodee only loads **ARM Linux** binaries (64-bit primary, 32-bit via bridge where supported).

## Hardware behavior (why “lightweight” matters)

| Topic | What to expect |
|-------|----------------|
| **UI** | Melodee is **headless** for plugin GUIs: parameters map to **K1–K3** and preset stepping; huge parameter lists mean more scrolling. |
| **CPU / RAM** | ~**2 GB RAM**, embedded **ARM** (see Open MPC `MPC_SAMPLE.md` for core class). Heavy unison or oversized editors can glitch or peg CPU. |
| **Incompatible files** | Wrong OS/arch → **dimmed** in the browser (not a crash). |
| **Audio** | Host uses embedded Linux audio (ALSA); reference stack in [sample-base](https://github.com/keremimo/sample-base) (~128-sample @ 44.1 kHz in community notes). |

## Design guidelines for embedded hosts

- **Audio thread**: no `malloc`/`free`, no locks, no I/O — same rules as hardware logue units, but float DSP is fine on ARM64.
- **Polyphony**: expose a low default max voices; document CPU cost in `plugins/<Name>/architecture.md`.
- **GUI**: prefer simple JUCE layouts; test at fixed resolution. If the device struggles with X11, consider `MPC_HEADLESS=1` when your plugin’s CMake supports `-DMPC_VST3_HEADLESS=ON` (editor stub, parameters via host only).
- **glibc**: binaries built on a very new Ubuntu may require a newer glibc than the MPC firmware. If a plugin fails to load despite correct `file` output, rebuild on an older LTS userspace or match Akai’s documented SDK/toolchain when available.

## What will **not** work

| Artifact | Result on MPC Sample |
|----------|----------------------|
| Windows `.vst3` (`x86_64-win` / `.dll`) | Will not load |
| macOS `.vst3` (`MacOS` / Mach-O) | Will not load |
| Linux x86_64 (`Contents/x86_64-linux`) | Will not load |
| Heavy unoptimized synths | May load but glitch or exhaust RAM/CPU |

## Upstream GitHub (research)

| Repo | Purpose |
|------|---------|
| [keremimo/melodee-sample](https://github.com/keremimo/melodee-sample) | Melodee host, plugin folder rules, Docker arm64 build |
| [keremimo/sample-base](https://github.com/keremimo/sample-base) | AC50 hardware runtime (ALSA, MIDI, display) |
| [keremimo/mpc-vst-melodee-*](https://github.com/keremimo/mpc-vst-melodee-chip) | Example lightweight synths + `*-mpc-sample.zip` |
| [inmusic-software/mpc-plugin-builder-sdk](https://github.com/inmusic-software/mpc-plugin-builder-sdk) | Official **MIT** MPC plugin SDK (VST2 ABI, JSON UI, AI skills, cross-build) |
| [open-mpc/mpc-vst-plugins](https://github.com/open-mpc/mpc-vst-plugins) | Community MPC OS ports; uses SDK as submodule; [`docs/MPC_SAMPLE.md`](https://github.com/open-mpc/mpc-vst-plugins/blob/main/docs/MPC_SAMPLE.md) |
| [inmusic-software/az0x-open-source](https://github.com/inmusic-software/az0x-open-source/tree/mpc-sample/1.3.0) | Published kernel/device-tree reference |

Fleet implementer: [docs/agents/LINUX_VST3_DSP_ENGINEER.md](agents/LINUX_VST3_DSP_ENGINEER.md).

## Related repo docs

- [vst/README.md](../vst/README.md) — JUCE boilerplate and desktop Linux builds
- [docs/vst-lifecycle.md](vst-lifecycle.md) — APC phases
- [disklordz/trap-lord-synth/Source/KeygroupExportOptions.h](../disklordz/trap-lord-synth/Source/KeygroupExportOptions.h) — MPC Live keygroup tuning (sibling product line, not VST3)

## CI note

Standard GitHub Actions `ubuntu-latest` jobs in this repo build **x86_64-linux** VST3 for pluginval. **aarch64** builds are operator-driven via `build-vst-aarch64-linux.sh` until a dedicated cross-compile workflow is added (requires workflow change via Cloud agent per repo policy).