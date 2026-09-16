# Instruments

CMake-native [JUCE](https://juce.com/) audio plugin starter (`MyFirstPlugin`).

## Project layout

```
MyFirstPlugin/
├── CMakeLists.txt
├── PluginProcessor.h
└── PluginProcessor.cpp
```

JUCE **7.0.12** is fetched automatically from GitHub when you configure CMake.

## Build (Cursor / VS Code)

1. Install **CMake Tools** and **C/C++** extensions.
2. Open the `MyFirstPlugin` folder (or the repo root and set the CMake source dir to `MyFirstPlugin`).
3. Configure: status bar **CMake: Configure** (or `cmake -B build -DCMAKE_BUILD_TYPE=Release`).
4. Build: **CMake: Build** (or `cmake --build build`).

On **macOS**, add `AU` to `FORMATS` in `CMakeLists.txt` for Audio Unit output:

```cmake
FORMATS VST3 AU Standalone
```

On **Linux**, `VST3` (and optionally `Standalone`) is supported; AU is macOS-only.

### Linux dependencies

Typical packages (Ubuntu/Debian):

```bash
sudo apt install build-essential libasound2-dev libfreetype-dev libgl1-mesa-dev \
  libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libgtk-3-dev
```

If the default `c++` is Clang without `libstdc++`, use GCC:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++
```

## Output

After a successful build, the VST3 bundle is under:

`MyFirstPlugin/build/MyFirstPlugin_artefacts/Release/VST3/MyFirstPlugin.vst3`

Copy that bundle into your DAW’s VST3 plugin folder and rescan plugins.

## Processor

`MyFirstPluginAudioProcessor` is a minimal pass-through effect with `juce::GenericAudioProcessorEditor` for a basic UI. Extend `processBlock` to add your DSP.
