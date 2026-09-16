# Instruments

CMake-native [JUCE](https://juce.com/) plugin starter (`MyFirstPlugin`). **You do not need Visual Studio Code** or the CMake/C++ marketplace extensions. Build from the terminal; use **Cursor**, **Claude Code**, or **Antigravity** only to edit sources and run commands.

## Where you work vs what compiles

| Tool | Role |
|------|------|
| **Cursor / Claude Code / Antigravity** | Edit `PluginProcessor.*`, `CMakeLists.txt`; run `build.ps1` or `build.sh` in the integrated terminal; use AI (Composer, etc.) on the repo |
| **CMake + compiler** | Actually builds the `.vst3` (Visual Studio Build Tools or MSVC on Windows, Xcode CLT on macOS, GCC/Clang on Linux) |

Cursor is a separate editor from VS Code. This repo does **not** assume you install VS Code or its extensions.

## Project layout

```
MyFirstPlugin/
├── CMakeLists.txt
├── ParameterIds.h
├── PluginProcessor.h
├── PluginProcessor.cpp
├── Build Plugin.bat   # Windows menu (double-click)
├── build-menu.ps1
├── CMakePresets.json  # Visual Studio preset
├── VISUAL_STUDIO.md
├── build.ps1
└── build.sh           # macOS / Linux
```

JUCE **7.0.12** is downloaded automatically on first configure.

## Build on Windows

Prerequisites: **Visual Studio 2022** with “Desktop development with C++” (includes MSVC; add **C++ CMake tools** in the installer). [CMake](https://cmake.org/download/) on PATH if you use scripts outside VS.

### Easiest: menu or Visual Studio GUI

- Double-click **`MyFirstPlugin/Build Plugin.bat`** → choose **3** (first time), then **2** after edits.  
- Or open the **`MyFirstPlugin`** folder in **Visual Studio** (File → Open → Folder) and use **Build**.  
- Full steps: **`MyFirstPlugin/VISUAL_STUDIO.md`**

### Terminal (Cursor)

```powershell
cd MyFirstPlugin
.\build.ps1
```

### macOS / Linux

```bash
cd MyFirstPlugin
chmod +x build.sh
./build.sh
```

Manual equivalent:

```bash
cd MyFirstPlugin
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

On **macOS**, add `AU` to `FORMATS` in `CMakeLists.txt` if you want Audio Unit:

```cmake
FORMATS VST3 AU Standalone
```

### Linux dependencies (if configure fails)

```bash
sudo apt install build-essential libasound2-dev libfreetype-dev libgl1-mesa-dev \
  libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libgtk-3-dev
```

## Output

**Windows:**  
`MyFirstPlugin\build\MyFirstPlugin_artefacts\Release\VST3\MyFirstPlugin.vst3`

**macOS / Linux:**  
`MyFirstPlugin/build/MyFirstPlugin_artefacts/Release/VST3/MyFirstPlugin.vst3`

Copy the bundle into your DAW’s VST3 folder and rescan.

## AI workflow (processor before UI)

1. Open this repo in **Cursor** (or clone it where Claude Code / Antigravity can see it).
2. Use the prompt in `MyFirstPlugin/CURSOR_COMPOSER_PROMPT.md` — same text works in any agent chat.
3. After code changes, run **`build.ps1`** or **`build.sh`** until the processor compiles.
4. Add a custom `PluginEditor` only after the processor is stable.

## Processor (production patterns)

`MyFirstPluginAudioProcessor` is a reference **VST3 effect** (stereo I/O):

- **APVTS** for parameters and preset/state XML
- **`ParameterIds.h`** — `std::string_view` IDs (no string lookups in `processBlock`)
- **Cached** `std::atomic<float>*` from `getRawParameterValue` in the constructor
- **`juce::LinearSmoothedValue`** on automatable controls
- **`juce::dsp::StateVariableTPTFilter`** + tube saturation + LFO → cutoff (real-time safe)

Custom **`PluginEditor`** with APVTS rotary controls is included.

## Quality-of-life in this repo

| Feature | Purpose |
|--------|---------|
| `Build Plugin.bat` / `build-menu.ps1` | CMake without memorizing commands |
| `CMakePresets.json` | Same configure in Visual Studio and scripts |
| `check-env.ps1` | Verifies CMake, MSVC, Git before first build |
| `MYFIRSTPLUGIN_COPY_AFTER_BUILD` | Optional auto-copy VST3 to the OS plugin folder |
| `.clangd` + `compile_commands.json` | Better go-to-definition in **Cursor** (after configure) |
| `.github/workflows/build-plugin.yml` | CI on Windows + Linux |
| `NEW_PLUGIN.md` | Rename / fork checklist for your next VST or VSTi |

**Possible next steps (not in repo yet):** pin JUCE with `GIT_TAG` you trust; add `Pluginval` in CI; split synth template (`IS_SYNTH TRUE`) as a second target when you need a VSTi.
