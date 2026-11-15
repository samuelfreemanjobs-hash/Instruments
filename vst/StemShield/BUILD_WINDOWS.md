# DiskLordz Stem-Shield — Windows build (Sam)

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
cd vst\StemShield
cmake -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target StemShield_VST3 StemShield_Standalone
```

Artefacts:

- VST3: `vst\StemShield\build\StemShield_artefacts\Release\VST3\DiskLordz Stem-Shield.vst3`
- Standalone: `vst\StemShield\build\StemShield_artefacts\Release\Standalone\`

## Processor tests (console)

```powershell
cmake --build build --config Release --target StemShieldProcessorTests
ctest --test-dir build -C Release -R StemShield --output-on-failure
```

## pluginval (optional)

Download [pluginval Windows release](https://github.com/Tracktion/pluginval/releases), then:

```powershell
$env:PLUGINVAL_BIN = "C:\path\to\pluginval.exe"
python ..\..\scripts\vst\run_pluginval.py --strictness 5 `
  --file "build\StemShield_artefacts\Release\VST3\DiskLordz Stem-Shield.vst3"
```

Strictness **5+** is recommended before shipping.

## DAW smoke

Load the `.vst3` on a stem export bus, confirm reported latency matches lookahead, and A/B peak-readings against your DAW meter. This agent does not perform DAW validation.
