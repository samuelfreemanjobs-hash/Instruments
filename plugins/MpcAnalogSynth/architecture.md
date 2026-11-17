# Architecture

- **Program chain:** `juce::Synthesiser` + `MpcSynthVoice` (dual `dsp::Oscillator`, `StateVariableTPTFilter`, `ADSR`).
- **Surface:** APVTS only; `hasEditor() == false`.
- **Voices:** 6 max (embedded CPU).
- **Build:** repo `vst/JUCE` submodule; cross aarch64 via `tools/build-vst-aarch64-linux.sh MpcAnalogSynth` with `MPC_HEADLESS=1` optional.