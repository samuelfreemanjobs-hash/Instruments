---
name: juce-release-qa
description: Release gate for a JUCE plugin or the drum engine. Builds Release, runs tests, and runs pluginval when a VST3 exists.
---

# JUCE release QA

Drum engine:

```bash
cmake -S disklordz/drum-engine -B disklordz/drum-engine/build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build disklordz/drum-engine/build --parallel
ctest --test-dir disklordz/drum-engine/build --output-on-failure
```

VST3, after that plugin's build:

```bash
python3 scripts/vst/run_pluginval.py PATH/TO/Plugin.vst3
```

On Linux, run pluginval under `xvfb-run -a` if the binary opens a window. A GitHub 504 while downloading pluginval is an infrastructure failure (`shared_incidents` I-0001). Re-run the job. Do not treat it as a DSP regression.

Factory presets must load from paths inside the repo or BinaryData. A missing sample is a failed gate.

`compile_commands.json` for this engine is `disklordz/drum-engine/build/compile_commands.json`. The repo `.clangd` still points at `MyFirstPlugin/build` for that product. Do not retarget `.clangd` globally.
