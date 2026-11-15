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
```

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

### Install on MPC Sample

Melodee’s exact scan path can vary by firmware; typical Linux layouts include user-level `~/.vst3/` or a vendor-specific plugin folder exposed over USB/network. Copy the **entire** `.vst3` bundle (folder), not the `.so` alone, then rescan plugins in Melodee.

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
| [open-mpc/mpc-vst-plugins](https://github.com/open-mpc/mpc-vst-plugins) | MPC OS ports; [`docs/MPC_SAMPLE.md`](https://github.com/open-mpc/mpc-vst-plugins/blob/main/docs/MPC_SAMPLE.md) hardware brief |
| [inmusic-software/az0x-open-source](https://github.com/inmusic-software/az0x-open-source/tree/mpc-sample/1.3.0) | Published kernel/device-tree reference |

Fleet implementer: [docs/agents/LINUX_VST3_DSP_ENGINEER.md](agents/LINUX_VST3_DSP_ENGINEER.md).

## Related repo docs

- [vst/README.md](../vst/README.md) — JUCE boilerplate and desktop Linux builds
- [docs/vst-lifecycle.md](vst-lifecycle.md) — APC phases
- [disklordz/trap-lord-synth/Source/KeygroupExportOptions.h](../disklordz/trap-lord-synth/Source/KeygroupExportOptions.h) — MPC Live keygroup tuning (sibling product line, not VST3)

## CI note

Standard GitHub Actions `ubuntu-latest` jobs in this repo build **x86_64-linux** VST3 for pluginval. **aarch64** builds are operator-driven via `build-vst-aarch64-linux.sh` until a dedicated cross-compile workflow is added (requires workflow change via Cloud agent per repo policy).