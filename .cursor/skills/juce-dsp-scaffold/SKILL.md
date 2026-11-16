---
name: juce-dsp-scaffold
description: Scaffolds a real-time JUCE DSP class split from the processor, with smoothing, oversampling, and a render test. Use for a new synth engine or sampler voice.
---

# JUCE DSP scaffold

1. Put the class in `disklordz/drum-engine/include/` if it is a drum voice. For an existing VST, use that plugin's `Source/Dsp/` only after Dream, Plan, and Design exist.
2. `prepare()` allocates. `process()` does not. Include `juce::SmoothedValue` for every continuous parameter and `juce::dsp::Oversampling` around nonlinear stages.
3. Add a case to `DrumEngineTests` or the plugin's existing test: impulse or note-on, no NaN, peak within range, tail near zero, scratch capacity unchanged.
4. Register parameters in APVTS and `params.json` with the same ids.
5. Build with the drum-engine CMake command in `drum-synth-dsp`, or `./tools/build-vst.sh <Name>` for an APC plugin.

Do not start a new `vst/<Name>/` tree from this skill.
