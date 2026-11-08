# Install your NTS-1 custom oscillators today

This repo ships **14 genre bass/kick oscillators** for the Korg NTS-1. Build once on your Mac/PC, then load via **KORG KONTROL Editor** (or loguecli on mkII).

## Quick path (Cursor or terminal)

```bash
cd /path/to/Instruments   # branch cursor/nts1-finish-install-2cf4 or merged main

# One-time toolchain
./tools/mkii-automate.sh bootstrap
# If ARM gcc download fails: sudo apt install gcc-arm-none-eabi   (Linux)

export LOGUE_SDK="$(pwd)/.deps/logue-sdk"

# NTS-1 original (mkI) — full catalog
./tools/build-all-bass.sh
./tools/package-nts1.sh
```

Install files: **`release/nts1-custom-oscillators/mkI-ntkdigunit/*.ntkdigunit`**

## NTS-1 mkII

After mkII ports are built:

```bash
./tools/build-all-bass-mkii.sh
./tools/package-nts1.sh
```

Files: **`release/nts1-custom-oscillators/mkII-nts1mkiiunit/*.nts1mkiiunit`**

Firmware **≥ 1.0.0** required for mkII SDK units.

## On the hardware

1. USB-connect the NTS-1.
2. Open **KORG KONTROL Editor** → your device → **Oscillator** user slot.
3. **Load** the `.ntkdigunit` or `.nts1mkiiunit` file.
4. **Send** / write to the synth.
5. Only **one** custom osc runs at a time — build or keep all packages on disk and swap per session.

## Presets (knob cheat sheet)

Each oscillator documents `_agent.presets` in `src/oscillators/<folder>/manifest.json` (values 0–100 unless noted).

Catalog names: see [nts1-multi-bass-oscillators.md](nts1-multi-bass-oscillators.md).

## Cursor IDE

Open this repo in Cursor, set `LOGUE_SDK` in the integrated terminal, run the commands above. Skills: `.cursor/skills/logue-hardware-port/SKILL.md`, rules under `.cursor/rules/05-multi-bass.mdc`.
