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

VST3, after that plugin's build. pluginval 1.0.4 uses `--validate`. Strictness 8 is the local loop. CI stays at 5 via `scripts/vst/run_pluginval.py`.

```bash
bin=$(python3 scripts/vst/run_pluginval.py --install-only)
xvfb-run -a "$bin" --strictness-level 8 --skip-gui-tests --validate PATH/TO/Plugin.vst3
```

On macOS or Windows, call `pluginval` or `pluginval.exe` on `PATH` with the same flags. A GitHub 504 while downloading pluginval is an infrastructure failure (`shared_incidents` I-0001). Re-run the download. Do not treat it as a DSP regression.

Factory presets must load from paths inside the repo or BinaryData. A missing sample is a failed gate.

`compile_commands.json` for this engine is `disklordz/drum-engine/build/compile_commands.json`. The repo `.clangd` still points at `MyFirstPlugin/build` for that product. Do not retarget `.clangd` globally.
