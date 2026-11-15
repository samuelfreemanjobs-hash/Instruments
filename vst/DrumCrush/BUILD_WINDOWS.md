# DiskLordz Drum-Crush — Windows build (Sam)

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
cd vst\DrumCrush
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target DrumCrush_VST3 DrumCrush_Standalone
```

Artefacts:

- VST3: `vst\DrumCrush\build\DrumCrush_artefacts\Release\VST3\DiskLordz Drum-Crush.vst3`
- Standalone: `vst\DrumCrush\build\DrumCrush_artefacts\Release\Standalone\`

## Processor tests (console)

```powershell
cmake --build build --config Release --target DrumCrushProcessorTests
ctest --test-dir build -C Release -R DrumCrush --output-on-failure
```

## pluginval (optional)

Download [pluginval Windows release](https://github.com/Tracktion/pluginval/releases), then:

```powershell
$env:PLUGINVAL_BIN = "C:\path\to\pluginval.exe"
python ..\..\scripts\vst\run_pluginval.py --strictness 5 `
  --file "build\DrumCrush_artefacts\Release\VST3\DiskLordz Drum-Crush.vst3"
```

Strictness **5** is used in CI before shipping.

## DAW smoke

Load the `.vst3` on a drum bus in your DAW, A/B parallel mix against KNOCK-style workflows. This agent does not perform DAW validation.
