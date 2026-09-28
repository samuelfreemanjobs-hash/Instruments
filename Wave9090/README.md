# WAVE-9090 v0.1.0

Sampleless wavetable synthesizer for dark trap and phonk (WAVE-9090 hybrid concept).

## Build

From repository root:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target Wave9090_VST3 Wave9090_Standalone Wave9090Tests
ctest --test-dir build -R Wave9090
```

## Docs

- [ARCHITECTURE.md](ARCHITECTURE.md)
- [Docs/PRODUCT_SPEC.md](Docs/PRODUCT_SPEC.md)
- [Docs/DSP_SPEC.md](Docs/DSP_SPEC.md)
