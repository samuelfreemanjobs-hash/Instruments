# Memphis Grit — architecture (APC Plan)

**Inputs:** [creative-brief.md](creative-brief.md) · [parameter-spec.md](parameter-spec.md)  
**Normative:** [docs/dsp-architecture-specification.md](../../../docs/dsp-architecture-specification.md)

## 1. Components

| Layer | Location | Role |
|-------|----------|------|
| APVTS | `vst/MemphisGrit/Source/PluginProcessor` | `mg_drive`, `mg_lofi`, `mg_mid`, `mg_mix`, `mg_output_db`, `mg_bypass` |
| Program chain | `Source/MemphisGritDsp.*` | Mid emphasis → tanh drive → bit crush → hold decimation → wet/dry → output trim + −0.5 dBFS ceiling |
| Editor | `Source/PluginEditor` | Five rotaries + bypass toggle; JUCE native (V1) |

## 2. Program chain

```text
per block:
  1. ScopedNoDenormals
  2. If bypass: copy input, apply output gain + ceiling only
  3. Else: MemphisGritDsp::process(in, out, numSamples)
  4. NaN/Inf → 0 at output
```

Stereo linked processing (same coefficients L/R). Latency 0. No oversampling in V1.

## 3. Real-time constraints

No heap in `processBlock`. Hold-decimator state is per-channel fixed arrays. Parameter reads from APVTS raw values (smoothing optional V2).

## 4. Build & test

`./tools/build-vst.sh MemphisGrit` · `ctest -R MemphisGrit` · ship via `./tools/package-vst.sh MemphisGrit 1.0.0`

Product `ARCHITECTURE.md`: `vst/MemphisGrit/ARCHITECTURE.md`
