# DiskLordz 808-Punisher

Phase 1 Harmonic Sub-Bass Enhancer (JUCE 8, VST3 + Standalone).

## Build (Linux)

```bash
git clone --branch 8.0.6 --depth 1 https://github.com/juce-framework/JUCE.git vst/JUCE
./tools/build-vst.sh Punisher808
ctest --test-dir vst/Punisher808/build -R Punisher808 --output-on-failure
```

Windows: see [BUILD_WINDOWS.md](BUILD_WINDOWS.md).

DSP notes: [plugins/Punisher808/DSP_ARCHITECTURE.md](../../plugins/Punisher808/DSP_ARCHITECTURE.md).
