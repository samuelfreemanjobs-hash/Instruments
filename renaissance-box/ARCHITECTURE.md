# MPC Renaissance dedicated box — architecture

## Purpose

Turn an **Akai MPC Renaissance** plus a **hidden mini PC** into a **pseudo-standalone** production appliance: power on → pads and audio work without opening a laptop. The Renaissance has **no replaceable onboard OS**; the “standalone” behavior is **host software on the mini PC** driving USB **MIDI + 4-in/2-out audio** (including phono inputs).

This package documents the box layout and ships a **Windows-first sidecar** for custom features alongside **Akai MPC Software** (required for the official Renaissance workflow).

## Build and run

### Sidecar (custom features, Windows or Linux)

```bash
cd renaissance-box/host
python3 -m venv .venv
# Windows: .venv\Scripts\activate
source .venv/bin/activate
pip install -r requirements.txt
python list_ports.py
python sidecar.py --config ../config/box.windows.json
```

Install **loopMIDI** (Windows) or use **PipeWire JACK/ALSA** (Linux) when bridging MIDI between MPC Software and the sidecar.

### Custom engine (optional, Linux-friendly)

Build **SP-1200 Drumulator** standalone from repo root (no Akai MPC Software on Linux):

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target SP1200_Standalone
```

Use this path when you want **100% owned application code** instead of MPC Software.

## Data flow

```text
                    ┌─────────────────────────────────────┐
  Renaissance USB ──┤ Mini PC                             │
                    │  ┌───────────────────────────────┐  │
                    │  │ Mode A (recommended box)     │  │
                    │  │  MPC Software (Windows)       │  │
                    │  │    ↔ Renaissance audio/MIDI   │  │
                    │  │  Sidecar (Python)             │  │
                    │  │    ↔ loopMIDI / virtual port  │  │
                    │  │    • Disklordz inbox hooks    │  │
                    │  │    • custom swing / macros    │  │
                    │  │    • LCD SysEx (future)       │  │
                    │  └───────────────────────────────┘  │
                    │  ┌───────────────────────────────┐  │
                    │  │ Mode B (fully custom)        │  │
                    │  │  Linux + SP1200_Standalone    │  │
                    │  │    ↔ Renaissance USB class    │  │
                    │  └───────────────────────────────┘  │
                    └─────────────────────────────────────┘
```

## Threading / realtime

| Component | Realtime notes |
|-----------|----------------|
| **MPC Software** | Akai handles audio; do not block its USB thread. |
| **Sidecar** | MIDI only — no audio I/O in v0; keep handlers short; no heavy work on callback thread. |
| **SP1200_Standalone** | Same rules as [SP1200/ARCHITECTURE.md](../SP1200/ARCHITECTURE.md): no allocation on audio thread. |

## Key modules

| Path | Responsibility |
|------|----------------|
| [docs/MPC_RENAISSANCE_DEDICATED_BOX.md](../docs/MPC_RENAISSANCE_DEDICATED_BOX.md) | Hardware, OS choice, autostart, operator checklist |
| `config/box.windows.json` | Sidecar ports, feature flags, MIDI map path |
| `config/midi-map.template.json` | Pad/CC map template (verify against your unit) |
| `host/list_ports.py` | Enumerate MIDI ports for setup |
| `host/sidecar.py` | Virtual-port bridge + extension hooks |
| `host/extensions/` | Pluggable feature modules |
| `scripts/windows/` | Autostart and task scheduler notes |
| `scripts/linux/` | Optional PipeWire + systemd user unit |

## Extension points

1. **`host/extensions/*.py`** — register in `sidecar.py`; implement `on_midi(msg)` / optional `tick()`.
2. **`config/midi-map.template.json`** — copy to `midi-map.local.json` after hardware verify; add note/CC rows.
3. **LCD / Q-Link rings** — capture SysEx while MPC Software drives the panel; add `host/lcd/` encoder module (not in v0).
4. **Disklordz** — point inbox watcher at box sample folder; sidecar `disklordz_inbox` extension fires on new WAV dirs.
5. **JUCE fork** — Renaissance device profile in SP1200 editor (future CMake target).

## Related docs

- [SP1200/ARCHITECTURE.md](../SP1200/ARCHITECTURE.md) — custom sampler/sequencer engine
- [disklordz/daw-inbox/ARCHITECTURE.md](../disklordz/daw-inbox/ARCHITECTURE.md) — ZIP → inbox automation
- [docs/BYTEBOT_SETUP.md](../docs/BYTEBOT_SETUP.md) — optional desktop agent on a separate dev machine
