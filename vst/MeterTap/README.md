# MeterTap

Stereo peak/RMS utility meter (DiskLordz VST Factory dogfood). Scaffolded via
`./tools/new-vst.sh MeterTap --type effect`.

## Build

JUCE is **not** a git submodule in this repo (no `.gitmodules` entry). Fetch it
once per machine with the same clone CI uses:

```bash
# From repo root — not committed; tools/build-vst.sh looks for vst/JUCE/
git clone --depth 1 --branch 7.0.12 https://github.com/juce-framework/JUCE.git vst/JUCE
./tools/build-vst.sh MeterTap
```

(`git submodule update --init vst/JUCE` will not work until a submodule is added.)

## Editor

V1 ships `juce::GenericAudioProcessorEditor` so the DSP/processor path can ship
before `vst/factory-lib/gui` lands. The custom editor (two `LevelMeter` bars,
bypass, optional trim, "Meters: input" label) follows
`plugins/MeterTap/.ideas/design-spec.md` once the design-system JUCE components
are available.

## Verify

```bash
plugins/MeterTap/scripts/verify-phase4.sh
# CI: xvfb-run -a bash plugins/MeterTap/scripts/verify-phase4.sh
```
