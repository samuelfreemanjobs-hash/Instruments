# MPC PlaySynth headless base

Starting point for **dual-layer Play Series** plugins on **MPC Sample** (Melodee).

## Create `vst/MpcPlayPhonk` (example)

```bash
cp -a templates/mpc-play-synth-base vst/MpcPlayPhonk
cd vst/MpcPlayPhonk
# Edit CMakeLists.txt: PRODUCT_NAME, PLUGIN_CODE Mp94
```

Engines are pulled in via include path to `templates/juce8-synth-base/Source/` — customize Layer A/B headers per SKU (see `plugins/MPC_PLAY_SYNTH_6PACK.md`).

```bash
./tools/build-vst.sh MpcPlayPhonk
./tools/build-vst-aarch64-linux.sh MpcPlayPhonk
```

## Melodee K1–K3

First three APVTS parameters in `PluginProcessor::createParameterLayout()` must match the SKU row in `mpc_play_synth_6pack_master.json`.