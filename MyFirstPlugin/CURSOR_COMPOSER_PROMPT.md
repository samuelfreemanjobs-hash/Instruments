# Cursor Composer prompt (processor-first)

Copy into **Composer** (`Ctrl+I` / `Cmd+I`). Fill in the bracketed fields.

```markdown
You are a senior Audio Developer specializing in modern C++ (C++17/20) and the JUCE framework.
We are building a production-ready, fully functional plugin inside Cursor for Windows.

[PROJECT METADATA]
- Plugin Name: [e.g., NexusDrive]
- Plugin Type: [Choose: VST3 Effect  OR  VSTi Synth Instrument]
- Audio Architecture: [e.g., Stereo Input/Output, or Midi-In to Stereo Out]
- Core Feature List: [e.g., 1. A state-variable lowpass filter, 2. An adjustable tube distortion stage, 3. An LFO modulating the cutoff]

[STRICT AUDIO THREAD CONSTRAINT]
You MUST write completely "Real-Time Safe" code inside `processBlock`.
1. Absolute NO dynamic memory allocations (no `std::vector::push_back`, no `new`, no `juce::String` manipulation).
2. NO system calls, NO UI updating, and NO console logging (`DBG()`).
3. NO locking of heavy mutexes or blocking operations.

[STRUCTURAL IMPLEMENTATION REQUIREMENTS]
1. STATE MANAGEMENT: Use a `juce::AudioProcessorValueTreeState` (APVTS) to handle all parameters. Define parameter IDs safely using `std::string_view` or inline constants to avoid allocation overhead.
2. DISCRETE CHANNELS & SMOOTHING: Ensure audio parameters are smoothed using `juce::LinearSmoothedValue` or `juce::SmoothedValue` to prevent audible digital clicks and pops during automation.
3. DSP MODULARITY: If using JUCE DSP modules (`juce::dsp`), properly initialize them inside `prepareToPlay` with a `juce::dsp::ProcessSpec` passing the sample rate, block size, and channel count.
4. SYNTH ONLY (If VSTi): Implement a complete MIDI handling loop. Use a standard `juce::Synthesiser` with custom `juce::SynthesiserVoice` and `juce::SynthesiserSound` structures, or write a raw per-sample MIDI buffer parsing block if explicit control is required.

[OUTPUT EXPECTATIONS]
- Provide clean, production-grade, highly commented code.
- Write the complete code without placeholders or "// implement your logic here" shortcuts.
- Break the execution into two separate phases: First, generate the complete code for `PluginProcessor.h` and `PluginProcessor.cpp`. Do not write the PluginEditor (GUI) until the processor code compiles cleanly.

Let's begin by generating the complete `PluginProcessor.h` first.
```

## Pro-tips

- **Composer** can edit `.h` and `.cpp` together; use it instead of inline chat for multi-file processor work.
- **Windows includes**: if paths break, ask: *Fix file includes using standard project paths matching a Windows environment.*
- **Parameter lookups**: cache `apvts.getRawParameterValue(...)` in the constructor (see `MyFirstPlugin/PluginProcessor.cpp`). Never call `getRawParameterValue("name")` inside `processBlock`.

This repo’s reference implementation: `MyFirstPlugin/` (effect demo matching the example feature list).
