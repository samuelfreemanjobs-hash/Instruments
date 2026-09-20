# SP-1200 Drumulator — architecture

Standalone **12-bit / 26.040 kHz** sampler inspired by the E-mu SP-1200 workflow. Product spec: [docs/SP1200_STANDALONE_SPEC.md](../docs/SP1200_STANDALONE_SPEC.md).

## Purpose

Performance sampler with **16 pads**, **16 voices**, **7:00** embedded sample RAM, WAV import, audio input record, **SQ-1 default MIDI map** (ch 10, notes 36–51, CC 20–35), keyboard transport (Space/Esc/R/Tab, Q–I / A–K pads).

## Build and run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target SP1200_Standalone SP1200MemoryTests SP1200SequencerTests SP1200ProjectFileTests
ctest --test-dir build -R SP1200
```

Artifact: `build/SP1200/SP1200_artefacts/Release/Standalone/SP-1200 Drumulator`

## Data flow

```text
WAV / input → SampleImporter (mono sum, resample) → TwelveBitBuffer → SampleMemoryPool
MIDI / UI pad → SamplerEngine → 16× SampleVoice (drop-sample pitch) → Ssm2044BusFilter → stereo out
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
| `Source/PluginEditor.*` | Console, MOD 11 chop overlay, save/load |
| `Source/UI/ChopModalComponent.*` | MOD 11 truncate / auto-chop / normalize |
| `Source/Project/ProjectFile.*` | `.sp12p` embedded samples + patterns |
| `Source/DSP/Ssm2044BusFilter.*` | Bus low-pass + per-step cutoff mod |

## MIDI & keyboard (defaults)

| Source | Mapping |
|--------|---------|
| SQ-1 row A/B | Ch **10**, notes **36–51** → pads 1–16 |
| SQ-1 knobs | Ch **10**, CC **20–35** → faders (VOL / PITCH / DECAY mode) |
| Keyboard | **Space** play, **Esc** stop, **R** step record (seq view), **Tab** fader mode, **Q–I / A–K** pads |

## Extension points

- Module 10 SETUP (MIDI Learn, device pick, clock)
- Choke groups (spec §2.5)
- Multi-pitch / chromatic cap via `quantizeToMultiPitch`

## Related

- [ARCHITECTURE.md](../ARCHITECTURE.md) (repo index)
