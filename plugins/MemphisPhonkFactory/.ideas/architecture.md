# Memphis Phonk Factory — architecture

## Targets

- `vst/MemphisPhonkFactory/` — JUCE VST3 + Standalone
- `Source/Dsp/MemphisPhonkEngine.*` — offline procedural renderer (Python parity)
- `shared/dsp/biquad.h` — RBJ filters for cassette chain

## Data flow

1. **Export (message thread):** UI → `MemphisPhonkEngine::exportMpcKit(dir)` → 23 WAV files @ 44.1 kHz mono.
2. **Preview (audio thread):** Message thread renders loop into `AudioBuffer<float>`; `processBlock` reads cyclically when preview is on.

## Threading

- No allocation on audio thread; loop buffer swapped via atomic pointer after regen on message thread / async updater.

## Build

```bash
./tools/build-vst.sh MemphisPhonkFactory
```
