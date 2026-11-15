# VST Factory — memory & realtime policy

## Audio thread

- No `new` / `malloc` / `std::vector` growth in `processBlock` or realtime MIDI handlers.
- No file, network, or lock waits on the audio thread.
- Preallocate delay lines, wavetables, and FIFO capacity in `prepareToPlay` / constructor.

## Shared DSP

Use `shared/dsp/` float helpers (`biquad.h`, `envelope_adsr.h`, `parameter_smoothing.h`, `phasor.h`) with fixed sizes. Document any scratch buffers in the design-spec.

## Factory-lib blocks

Blocks under `vst/factory-lib/` must declare `realtime_safe: true` in module JSON only when they meet this policy. Metering (`analysis.peak_rms`) is analysis-only and must not allocate per sample.

## Plugin state

- APVTS / atomics for UI → DSP parameter handoff.
- Heavy preset loads happen on the message thread; swap prepared graphs with a lock-free or double-buffer pattern.

## Factory service

The FastAPI orchestrator is **not** realtime. Subprocess builds may allocate freely; generated C++ must still obey this document.
