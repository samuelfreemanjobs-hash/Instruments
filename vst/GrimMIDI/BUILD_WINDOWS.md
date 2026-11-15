# DiskLordz Grim-MIDI — Windows build (Sam)

MIDI effect (VST3 + Standalone). One key triggers dark minor / phrygian-family voicings; progression presets and drag-export MIDI from the GUI.

## Prerequisites

- Visual Studio 2022 (Desktop development with C++)
- CMake 3.22+
- Git

## One-time JUCE fetch

From monorepo root:

```powershell
git clone --branch 8.0.6 --depth 1 https://github.com/juce-framework/JUCE.git vst\JUCE
```

## Configure & build

```powershell
cd vst\GrimMIDI
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target GrimMIDI_VST3 GrimMIDI_Standalone GrimMIDIProcessorTests
```

Artefacts:

- VST3: `vst\GrimMIDI\build\GrimMIDI_artefacts\Release\VST3\DiskLordz Grim-MIDI.vst3`
- Standalone: `vst\GrimMIDI\build\GrimMIDI_artefacts\Release\Standalone\`

## Processor contract tests

```powershell
ctest --test-dir build -C Release -R GrimMIDIProcessorContracts --output-on-failure
```

## pluginval (optional)

```powershell
$env:PLUGINVAL_BIN = "C:\path\to\pluginval.exe"
python ..\..\scripts\vst\run_pluginval.py --strictness 5 `
  --file "build\GrimMIDI_artefacts\Release\VST3\DiskLordz Grim-MIDI.vst3"
```

## DAW routing caveats

VST3 MIDI effects are not uniformly supported across hosts.

| DAW | Typical routing |
|-----|-----------------|
| **FL Studio** | Insert on a **Generator** (instrument) slot with MIDI out enabled, or use Patcher MIDI routing. Some builds list MIDI FX only on instrument tracks — if missing, load as instrument and disable audio path. |
| **Ableton Live** | Place on a MIDI track **before** the instrument via a MIDI-effect chain. Live 11+ supports VST3 MIDI effects on MIDI tracks; audio passthrough is silent stereo. |
| **Reaper** | Dedicated **MIDI-only track** or FX container with MIDI routing to instrument track. |

Grim-MIDI sets `IS_MIDI_EFFECT`, `NEEDS_MIDI_INPUT`, and `NEEDS_MIDI_OUTPUT` for correct JUCE/VST3 classification. If a host cannot load native MIDI FX, use the **Standalone** app to drag-export progression MIDI into the DAW.

This agent does not perform in-DAW validation on your machine.
