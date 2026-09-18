# WAVE-909 v0.1.0

Sampleless wavetable synthesizer for dark trap and phonk (Analog/Digital Hybrid Engine).

## Build

From repository root:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target Wave909_VST3 Wave909_CLAP Wave909_Standalone \
  Wave909OfflineRender Wave909Tests Wave909StateTests
ctest --test-dir build -R Wave909
bash Wave909/tests/golden/verify_golden.sh
```

## Release packaging

```bash
bash Wave909/Scripts/package_release.sh
# Output: Wave909/Release/ (VST3, CLAP, Standalone, docs, SHA256SUMS)
```

## Docs

- [ARCHITECTURE.md](ARCHITECTURE.md)
- [Docs/USER_MANUAL.md](Docs/USER_MANUAL.md)
- [Docs/VALIDATION_REPORT.md](Docs/VALIDATION_REPORT.md)
- [Docs/PRODUCT_SPEC.md](Docs/PRODUCT_SPEC.md)
- [Docs/DSP_SPEC.md](Docs/DSP_SPEC.md)
