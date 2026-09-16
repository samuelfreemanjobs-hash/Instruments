# Visual Studio as your CMake interface

You do **not** need to memorize CMake commands. Visual Studio 2022 (with **Desktop development with C++**) is a full graphical front-end for this project.

## Option A — Easiest: menu script (Cursor terminal or double-click)

1. In File Explorer, open the `MyFirstPlugin` folder.
2. Double-click **`Build Plugin.bat`**.
3. Press **3** (Configure + Build) the first time; later use **2** after code changes.
4. Press **4** to open the `.vst3` in Explorer.

Same menu from PowerShell:

```powershell
cd path\to\Instruments\MyFirstPlugin
.\build-menu.ps1
```

## Option B — Visual Studio buttons (recommended while VS is installing)

1. Open **Visual Studio 2022**.
2. **File → Open → Folder…**
3. Select the **`MyFirstPlugin`** folder (the one that contains `CMakeLists.txt`).
4. Wait for CMake to finish configuring (status bar at the bottom). First run downloads JUCE (can take several minutes).
5. Toolbar: set configuration to **Release** and **x64**.
6. **Build → Build All** (or right-click target **MyFirstPlugin_VST3** in CMake Targets and **Build**).

Output:

`MyFirstPlugin\build\MyFirstPlugin_artefacts\Release\VST3\MyFirstPlugin.vst3`

### Presets dropdown

This repo includes `CMakePresets.json`. In Visual Studio you may see:

- **Windows — Release (VST3)** (`windows-vs-release`)

Select it, then **Configure** / **Build** from the CMake menu if your VS version shows preset controls.

## Option C — Cursor for code, VS only to build

- Edit `PluginProcessor.cpp` in **Cursor** (AI, Composer).
- Switch to **Visual Studio** with the same folder open and press **Build**.

Both editors use the same `build` folder.

## What to install in Visual Studio

Workload: **Desktop development with C++**

Individual components (if prompted):

- **MSVC** toolset
- **Windows SDK**
- **C++ CMake tools for Windows** (CMake integration)

Optional: **Ninja** (faster CLI builds via `build.ps1`).

## Troubleshooting

| Problem | Fix |
|--------|-----|
| CMake not found | Install [CMake](https://cmake.org/download/) or enable **C++ CMake tools** in VS Installer |
| Configure fails on JUCE | Check internet; firewall must allow GitHub |
| No `MyFirstPlugin_VST3` target | Configure succeeded? Open **CMake Targets View** |
| DAW does not see plugin | Copy entire `.vst3` **folder** to `C:\Program Files\Common Files\VST3\` (or your DAW’s path) |
