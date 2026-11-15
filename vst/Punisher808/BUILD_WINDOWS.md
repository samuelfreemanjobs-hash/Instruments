# DiskLordz 808-Punisher — Windows build (Sam)

## Prerequisites

- **Visual Studio 2022** (Desktop development with C++)
- **CMake 3.22+** (bundled with VS or from cmake.org)
- Git

## One-time JUCE fetch

From the monorepo root (`Instruments`):

```powershell
git clone --branch 8.0.6 --depth 1 https://github.com/juce-framework/JUCE.git vst\JUCE
```

(`vst/JUCE` is gitignored; each machine clones locally.)

## Configure & build

```powershell
cd vst\Punisher808
cmake -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target Punisher808_VST3 Punisher808_Standalone
```

Artefacts:

- VST3: `vst\Punisher808\build\Punisher808_artefacts\Release\VST3\DiskLordz 808-Punisher.vst3`
- Standalone: `vst\Punisher808\build\Punisher808_artefacts\Release\Standalone\`

## Processor tests (console)

```powershell
cmake --build build --config Release --target Punisher808ProcessorTests
ctest --test-dir build -C Release -R Punisher808 --output-on-failure
```

## pluginval (optional)

Download [pluginval Windows release](https://github.com/Tracktion/pluginval/releases), then:

```powershell
$env:PLUGINVAL_BIN = "C:\path\to\pluginval.exe"
python ..\..\scripts\vst\run_pluginval.py --strictness 5 `
  --file "build\Punisher808_artefacts\Release\VST3\DiskLordz 808-Punisher.vst3"
```

Strictness **5+** is recommended before shipping.

## DAW smoke

Load the `.vst3` in your DAW (Reaper, FL, etc.), insert on an 808 bus, compare phone playback A/B. This agent does not perform DAW validation.
