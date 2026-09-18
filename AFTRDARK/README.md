# AFTRDARK v0.1.0

Sampleless wavetable synthesizer for dark trap and phonk (WAVE-909 hybrid concept).

## Build

From repository root:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target AFTRDARK_VST3 AFTRDARK_Standalone AftrdarkTests
ctest --test-dir build -R Aftrdark
```

## Docs

- [ARCHITECTURE.md](ARCHITECTURE.md)
- [Docs/PRODUCT_SPEC.md](Docs/PRODUCT_SPEC.md)
- [Docs/DSP_SPEC.md](Docs/DSP_SPEC.md)
