# JUCE Renaissance Host — architecture (A)

Custom **standalone application** for the MPC Renaissance pseudo-standalone box. This is the primary place to add **your own features** (sampler, sequencer, LCD logic) without SP-1200 or Akai firmware changes.

## Purpose

| Layer | Role |
|-------|------|
| **Windows box v1** | Akai MPC Software + Renaissance USB audio (see [BOX_V1_CHECKLIST.md](../renaissance-box/docs/BOX_V1_CHECKLIST.md)) |
| **JUCE host (this doc)** | Your C++ app: MIDI from Renaissance → custom audio engine → Renaissance USB audio out |
| **Python sidecar (optional)** | MIDI sniff / folder hooks only — not the main product |

## Build and run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target RenaissanceHost_Standalone RenaissanceHostSurfaceMapTests
ctest --test-dir build -R RenaissanceHostSurfaceMap --output-on-failure
```

Standalone binary (path varies by OS):

`build/renaissance-box/juce-host/RenaissanceHost_artefacts/Release/Standalone/Renaissance Host`

On the **dedicated box**, set Windows or Linux audio I/O to **MPC Renaissance**, then launch this app instead of (or alongside) MPC Software when you are on the custom-engine path.

## Data flow

```text
Renaissance USB MIDI (pads, Q-Links, transport*)
        ↓
RenaissanceSurfaceMap (notes 36–51, CC 1–16, omni)
        ↓
PluginProcessor::handleMidiMessage
        ↓
PadVoiceEngine (placeholder sine) → stereo out → Renaissance USB audio
        ↓
LcdBridge (SysEx stub → future LCD lines)

* Transport mapping: add in Source/ when captured from hardware.
```

## Key modules

| Path | Responsibility |
|------|----------------|
| `renaissance-box/juce-host/Source/RenaissanceSurfaceMap.*` | Pad/Q-Link MIDI map |
| `renaissance-box/juce-host/Source/PadVoiceEngine.*` | Replace with sampler/sequencer DSP |
| `renaissance-box/juce-host/Source/LcdBridge.*` | SysEx LCD hook (stub) |
| `renaissance-box/juce-host/Source/PluginProcessor.*` | Audio/MIDI realtime glue |
| `renaissance-box/juce-host/Source/PluginEditor.*` | Surface feedback UI |

## Extension points

1. **Swap `PadVoiceEngine`** for disk streaming, SP-style timing, or plugin hosting.
2. **Implement `LcdBridge::sendStatusLine`** after SysEx capture (compare with MPC Software traffic).
3. **Persist projects** — add `ValueTree` or custom JSON beside `.mpc` import/export.
4. **Box autostart** — systemd (Linux) or Task Scheduler (Windows) pointing at `Renaissance Host` binary.
5. **Disklordz** — watch inbox folder from a background `juce::Thread` and load WAV into voices.

## Related

- [MPC_RENAISSANCE_DEDICATED_BOX.md](MPC_RENAISSANCE_DEDICATED_BOX.md) — hardware + OS
- [renaissance-box/ARCHITECTURE.md](../renaissance-box/ARCHITECTURE.md) — package index
