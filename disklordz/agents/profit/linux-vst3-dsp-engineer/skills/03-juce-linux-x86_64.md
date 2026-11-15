# Skill 03 — JUCE Linux x86_64 (desktop / CI)

## Submodule

```bash
git submodule update --init vst/JUCE
# Pin: JUCE_VERSION file → 7.0.12 for factory lane
```

## Configure & build (single plugin)

```bash
./tools/new-vst.sh MyPlugin          # once, after APC design
./tools/build-vst.sh MyPlugin
```

Manual:

```bash
cd vst/MyPlugin
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12
cmake --build build --config Release -j"$(nproc)" --target MyPlugin_VST3
```

Artifact: `build/*_artefacts/Release/VST3/*.vst3/Contents/x86_64-linux/*.so`

## QA stack

```bash
python3 vst-testing-ops/run_business.py --profile ci        # before push when C++ changed
python3 vst-testing-ops/run_business.py --profile dsp-only  # faster DSP-only
```

pluginval runs under **xvfb** in CI — match apt deps from `.github/workflows/build.yml`.

## Common failures [inspected]

| Symptom | Fix |
|---------|-----|
| JUCE missing | `git submodule update --init vst/JUCE` |
| pluginval GUI | `xvfb-run -a` |
| Wrong JUCE API | Match `/JUCE_VERSION`; guard 8-only APIs |
| `c++` is clang without libstdc++ | Use `g++-12` / `g++-13` explicitly |

## CMake identity

Unique `PLUGIN_MANUFACTURER_CODE` / `PLUGIN_CODE` per product in `CMakeLists.txt`.