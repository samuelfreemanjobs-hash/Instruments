# SP-1200 Drumulator — architecture

Standalone **12-bit / 26.040 kHz** sampler inspired by the E-mu SP-1200 workflow. Product spec: [docs/SP1200_STANDALONE_SPEC.md](../docs/SP1200_STANDALONE_SPEC.md).

## Purpose

Performance sampler with **16 pads**, **16 voices**, **7:00** embedded sample RAM, WAV import, audio input record, SQ-1-friendly MIDI maps (in progress).

## Build and run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target SP1200_Standalone SP1200MemoryTests SP1200SequencerTests
ctest --test-dir build -R SP1200
```

Artifact: `build/SP1200/SP1200_artefacts/Release/Standalone/SP-1200 Drumulator`

## Data flow

```text
WAV / input → SampleImporter (mono sum, resample) → TwelveBitBuffer → SampleMemoryPool
MIDI / UI pad → SamplerEngine → 16× SampleVoice (drop-sample pitch) → stereo out
```

## Threading

| Thread | Work |
|--------|------|
| Audio | `processBlock`, voice render, input record append — **no alloc** |
| Message | Editor, file import, chooser |
| Background | Future: transient chop, project encode |

## Key modules

| Path | Responsibility |
|------|----------------|
| `Source/SP1200Constants.h` | 26.040 kHz, 7:00 cap, pad count |
| `Source/Memory/*` | 12-bit buffers, RAM pool |
| `Source/Import/SampleImporter.*` | File/buffer → SP format |
| `Source/Engine/SamplerEngine.*` | MIDI, voices, record |
| `Source/PluginProcessor.*` | JUCE standalone shell |
| `Source/PluginEditor.*` | Console UI v0 (16 faders/pads) |

## Extension points

- MOD 11 waveform chop modal (transient chop engine done)
- MOD 20 **PianoRollComponent** (16 lanes, click toggle steps, playhead, chromatic cap)
- Song tab: 8-slot chain; pattern bank 99, bars 1–4
- Multi-pitch / chromatic cap via `quantizeToMultiPitch`
- Project `.sp12p` embed in `getStateInformation`
- Bus SSM2044 filter (Module 15)

## Related

- [ARCHITECTURE.md](../ARCHITECTURE.md) (repo index)
