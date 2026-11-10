# DiskLordz Grim-MIDI

**Dark Minor & Phrygian Chord Generator with 1-Key Strum** (DiskLordz launch plan Phase 4).

## Features

- MIDI effect: one incoming note → dark minor / harmonic minor / phrygian / phrygian dominant voicing
- Parameters: key, scale, chord type, inversion, spread, strum time, velocity humanize, octave
- **36** dark hip-hop progression presets (starter set; structure supports expansion toward 100)
- GUI **Drag MIDI to DAW** exports the active progression as a `.mid` file
- Sample-accurate strum scheduling; note-off balanced voice tracking; panic on parameter changes, transport stop, and CC 123

## Build

See [BUILD_WINDOWS.md](BUILD_WINDOWS.md) and root `AGENTS.md` (Linux: g++-12, JUCE 8.0.6 under `vst/JUCE`).

```bash
cd vst/GrimMIDI
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target GrimMIDI_VST3 GrimMIDIProcessorTests
ctest --test-dir build -R GrimMIDIProcessorContracts --output-on-failure
```
