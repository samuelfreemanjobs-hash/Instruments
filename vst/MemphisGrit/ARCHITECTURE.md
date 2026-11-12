# Memphis Grit (VST3 + Standalone)

DiskLordz FX — lo-fi distortion and vintage-sampler degradation.

## Build

```bash
./tools/build-vst.sh MemphisGrit
ctest --test-dir vst/MemphisGrit/build -R MemphisGrit --output-on-failure
```

VST3: `vst/MemphisGrit/build/MemphisGrit_artefacts/Release/VST3/Memphis Grit.vst3`

## Data flow

```
Input → (bypass? gain/ceiling only)
     → MemphisGritDsp: mid emphasis → drive → quantize → hold-decimate → mix → output trim
```

## Modules

| Path | Role |
|------|------|
| `Source/MemphisGritDsp.*` | Program chain DSP |
| `Source/PluginProcessor.*` | APVTS + bypass |
| `Source/PluginEditor.*` | Five-knob UI |
| `tests/ProcessorTests.cpp` | Headless contracts |
