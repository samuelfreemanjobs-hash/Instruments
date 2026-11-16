---
name: build-plugin
description: Configures and builds one JUCE or drum-engine target, then reports the artifact. Use when the user says build the plugin or compile the drum engine.
---

# Build plugin

Build one target. Do not configure the whole monorepo unless the user asked for that.

Drum engine (no system VST3 copy):

```bash
cmake -S disklordz/drum-engine -B disklordz/drum-engine/build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12 \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build disklordz/drum-engine/build --target DrumEngineTests DrumBatchExporter
```

An existing APC/VST target:

```bash
bash tools/build-vst.sh <Name>
```

Windows laptop (one target, copy-after-build off): see `.cursor/rules/windows-worker.mdc`.

Report the artifact path, the last compiler error if the build failed, and the exact test command. Delegate the long log to `build-runner`.
