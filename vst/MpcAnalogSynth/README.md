# MpcAnalogSynth

Headless **6-voice** analog-style polysynth for **Melodee** (MPC Sample): dual oscillators, SVF lowpass, ADSR. No plugin editor (`hasEditor() == false`).

## Build (desktop x86_64)

```bash
git submodule update --init vst/JUCE
./tools/build-vst.sh MpcAnalogSynth
```

## Build (aarch64 for Melodee)

```bash
MPC_HEADLESS=1 ./tools/build-vst-aarch64-linux.sh MpcAnalogSynth
./scripts/vst/verify-mpc-sample-vst3.sh vst/MpcAnalogSynth/build-aarch64/MpcAnalogSynth_artefacts/Release/VST3/MpcAnalogSynth.vst3
```

Copy the whole `MpcAnalogSynth.vst3` folder to `Melodee/Plugins/` on the microSD card.

## Podman / dockcross (optional)

Same tree; mount repo and use `dockcross/linux-arm64` with `-DCMAKE_TOOLCHAIN_FILE` replaced by the image’s toolchain, or use the repo script above on a Linux host with `gcc-aarch64-linux-gnu`.

## Programs

Host program change: **Init**, **Warm Pad**, **Acid Lead**, **Soft Brass** (see `PluginProcessor.cpp`).