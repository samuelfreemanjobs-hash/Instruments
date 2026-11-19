# Architecture

- JUCE `AudioProcessor` MIDI effect (`IS_MIDI_EFFECT`), stereo passthrough audio (silence).
- APVTS mirrors SynthForge normalized parameters; `processBlock` emits CC when values change or on Send.
- State XML = full APVTS for DAW recall → re-send CC on load.
- CC map from `tools/synth-forge/synth_forge/data/minilogue_xd/midi_cc_map.json`.
