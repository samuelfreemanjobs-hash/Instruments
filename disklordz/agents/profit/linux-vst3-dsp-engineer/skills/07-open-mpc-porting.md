# Skill 07 — Open MPC porting (MPC OS lane)

## When to use

User wants **native MPC touchscreen page**, Q-Links, and **VST2** on MPC Live / One / Force — **not** Melodee JUCE VST3.

Stack: [open-mpc/mpc-vst-plugins](https://github.com/open-mpc/mpc-vst-plugins) + optional [Akai MPC Plugin SDK](https://github.com/inmusic-software/mpc-plugin-builder-sdk).

## When to stay on JUCE VST3 (this agent default)

- Target is **Melodee** on MPC Sample with standard VST3 bundles
- Target is **Linux DAW** (Reaper, Bitwig, etc.)
- Product already scaffolded under `vst/<Name>/` in Instruments

## Open MPC workflow (external repo)

```
vst.json → tools/build_port.sh → armv7 + aarch64 .so + MPC skin
Optional: VST3_TARGET=linux tools/build_vst3.sh for desktop DAW
```

Melodee Chip example `vst.json`:

- `targets`: `["armv7","aarch64"]`
- `build.cflags_arm`: `-mcpu=cortex-a17` (Gen1); aarch64 for Sample/Gen2
- Docker build via sibling `mpc-vst-plugins` checkout

## Desktop VST3 from Open MPC [inspected]

`docs/DESKTOP_VST3.md` in mpc-vst-plugins: hand-written VST3 wrapper, `Contents/aarch64-linux/` when built on arm64 Linux — **different** from JUCE bundle layout but same Steinberg folder convention.

## Hybrid products

Do **not** merge Open MPC VST2 wrapper and JUCE VST3 in one CMake tree without explicit architecture doc. Prefer:

- JUCE product in Instruments `vst/`
- Or Open MPC port in its own repo with `mpc-vst-plugins` toolchain

## Catalogue / install

- MPC OS 3.10.1+: `VST2 Plugins/` folder on drives
- Plugin Manager VST: [open-mpc/plugin-manager](https://github.com/open-mpc/plugin-manager)

This agent may **read** Open MPC docs to inform embedded limits; implementation happens in Open MPC tree unless user asks for a bridge doc only.