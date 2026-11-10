# DiskLordz Stem-Shield

Sub-bus peak containment and master stem balancer for beat stem exports.

- **Ceiling:** default -0.5 dBTP with lookahead true-peak limiting
- **Sub Balance:** dynamic sub-band gain vs full-band energy
- **~20 factory presets** for BeatStars / Airbit / artist delivery workflows

See `plugins/StemShield/DSP_ARCHITECTURE.md` for signal-path reasoning and `BUILD_WINDOWS.md` for Sam's Windows build steps.

Build (Linux/macOS):

```bash
git clone --branch 8.0.6 --depth 1 https://github.com/juce-framework/JUCE.git ../JUCE
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j --target StemShield_VST3 StemShieldProcessorTests
ctest --test-dir build -R StemShieldProcessorContracts --output-on-failure
```
