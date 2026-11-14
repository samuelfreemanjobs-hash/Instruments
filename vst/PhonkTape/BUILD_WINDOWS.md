# DiskLordz Phonk-Tape — Windows build (Sam)

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
cd vst\PhonkTape
cmake -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target PhonkTape_VST3 PhonkTape_Standalone
```

Artefacts:

- VST3: `vst\PhonkTape\build\PhonkTape_artefacts\Release\VST3\DiskLordz Phonk-Tape.vst3`
- Standalone: `vst\PhonkTape\build\PhonkTape_artefacts\Release\Standalone\`

## Processor tests (console)

```powershell
cmake --build build --config Release --target PhonkTapeProcessorTests
ctest --test-dir build -C Release -R PhonkTape --output-on-failure
```

## pluginval (optional)

Download [pluginval Windows release](https://github.com/Tracktion/pluginval/releases), then:

```powershell
$env:PLUGINVAL_BIN = "C:\path\to\pluginval.exe"
python ..\..\scripts\vst\run_pluginval.py --strictness 5 `
  --file "build\PhonkTape_artefacts\Release\VST3\DiskLordz Phonk-Tape.vst3"
```

Strictness **5+** is recommended before shipping.

## DAW smoke

Load the `.vst3` in your DAW (Reaper, FL, etc.), insert on a phonk bus, A/B wow/flutter and drive. This agent does not perform DAW validation.
