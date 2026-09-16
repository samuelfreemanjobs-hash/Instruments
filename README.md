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

### Install extensions in Cursor

1. Open the **Extensions** view: `Ctrl+Shift+X` (Windows/Linux) or `Cmd+Shift+X` (macOS).
2. Search and install:
   - **CMake Tools** (`ms-vscode.cmake-tools`)
   - **C/C++** (`ms-vscode.cpptools`)
3. Reload Cursor if prompted.

Opening this repo should also show **“Install recommended extensions”** (from `.vscode/extensions.json`). Click **Install**.

Command line (optional, if the `cursor` CLI is on your PATH):

```bash
cursor --install-extension ms-vscode.cmake-tools
cursor --install-extension ms-vscode.cpptools
```

### Configure and build

1. Open the repo root (CMake source is set to `MyFirstPlugin` in `.vscode/settings.json`).
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

## Processor (production patterns)

`MyFirstPluginAudioProcessor` is a reference **VST3 effect** (stereo I/O) that matches the Composer prompt in `MyFirstPlugin/CURSOR_COMPOSER_PROMPT.md`:

- **APVTS** for parameters and preset/state XML
- **`ParameterIds.h`** — `std::string_view` IDs (no string lookups in `processBlock`)
- **Cached** `std::atomic<float>*` from `getRawParameterValue` in the constructor
- **`juce::LinearSmoothedValue`** on all automatable controls
- **`juce::dsp::StateVariableTPTFilter`** + tube saturation + LFO → cutoff (real-time safe loop)

UI phase: `GenericAudioProcessorEditor` binds APVTS automatically; replace with `PluginEditor` when you are ready.
