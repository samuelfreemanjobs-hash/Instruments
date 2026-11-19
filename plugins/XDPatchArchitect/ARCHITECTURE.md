# XDPatchArchitect — architecture

## Purpose

VST3 MIDI effect that sends Korg Minilogue XD Control Change messages from DAW automation and UI randomize/mutate controls.

## Build & run

```bash
cd vst/XDPatchArchitect
# Requires vst/JUCE — see MeterTap BUILD notes
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target XDPatchArchitect_VST3
```

## Data flow

```text
APVTS parameter change → PluginProcessor::appendCcIfChanged → MidiBuffer out
Category / Mutate buttons → pseudo-random offsets → CC burst on Send
```

## Related docs

- [tools/synth-forge/synth_forge/data/minilogue_xd/midi_cc_map.json](../../tools/synth-forge/synth_forge/data/minilogue_xd/midi_cc_map.json)
