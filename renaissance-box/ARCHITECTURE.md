# MPC Renaissance dedicated box — architecture

## Purpose

**Pseudo-standalone** rig: MPC Renaissance + hidden mini PC. Custom features live in **JUCE** ([`juce-host/`](juce-host/)), not in Akai firmware. Optional Python [`host/`](host/) sidecar is for MIDI sniffing and inbox hooks only.

## Build and run

### JUCE host (primary — custom engine)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target RenaissanceHost_Standalone
ctest --test-dir build -R RenaissanceHostSurfaceMap --output-on-failure
```

Deep dive: [docs/JUCE_RENAISSANCE_HOST.md](../docs/JUCE_RENAISSANCE_HOST.md).

### Python sidecar (optional dev / bridge)

```bash
cd renaissance-box/host && pip install -r requirements.txt
python sidecar.py --config ../config/box.windows.json --log-midi
```

## Data flow

```text
┌─────────────────────────────────────────────────────────────┐
│ Mini PC                                                     │
│  Box v1: MPC Software (Windows) + Renaissance audio         │
│  Custom: Renaissance Host (JUCE) ← pads/CC → your DSP       │
│  Optional: Python sidecar (MIDI log, Disklordz poll)        │
└─────────────────────────────────────────────────────────────┘
          USB MIDI + audio
                ↓
         MPC Renaissance
```

## Threading / realtime

| Component | Notes |
|-----------|--------|
| **JUCE audio callback** | No allocation in `processBlock`; extend `PadVoiceEngine` accordingly. |
| **Python sidecar** | MIDI only; not on audio path. |
| **MPC Software** | Use for v1 validation only when not running JUCE host. |

## Key modules

| Path | Role |
|------|------|
| [juce-host/](juce-host/) | **Renaissance Host** standalone (A + C product code) |
| [docs/BOX_V1_CHECKLIST.md](docs/BOX_V1_CHECKLIST.md) | Windows + Akai sign-off |
| [config/](config/) | Sidecar JSON + shared MIDI map template |
| [host/](host/) | Optional Python bridge |
| [scripts/windows/](scripts/windows/) | Autostart notes |

## Extension points

See [docs/JUCE_RENAISSANCE_HOST.md](../docs/JUCE_RENAISSANCE_HOST.md).

## Related docs

- [docs/MPC_RENAISSANCE_DEDICATED_BOX.md](../docs/MPC_RENAISSANCE_DEDICATED_BOX.md)
- [docs/BYTEBOT_SETUP.md](../docs/BYTEBOT_SETUP.md)
