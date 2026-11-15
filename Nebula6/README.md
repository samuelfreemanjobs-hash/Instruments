# Nebula6

Remarkable Juno-family synth for cloud phonk / phonkwave — APC: `plugins/Nebula6/`.

**Status:** Scaffold from `templates/juce8-synth-base` (Play shell). Replace Layer A/B with `NebulaEngine` per `plugins/Nebula6/.ideas/architecture.md`.

```bash
cmake -S Nebula6 -B Nebula6/build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12 \
  -DPLAY_JUCE_DIR="$(pwd)/vst/JUCE" -DPLAY_SYNTH_HEADLESS_CI=ON
cmake --build Nebula6/build -j --target Nebula6_VST3
Nebula6/build/Nebula6_artefacts/Release/Standalone/Nebula6 --render-test
```