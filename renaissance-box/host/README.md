# Renaissance box — host sidecar

Lightweight **MIDI sidecar** for a Windows dedicated box running **MPC Software** + **MPC Renaissance**. Use it to add features Akai does not ship (folder watchers, custom CC transforms, logging) without modifying MPC Software.

## Setup (Windows box)

```powershell
cd renaissance-box\host
python -m venv .venv
.\.venv\Scripts\activate
pip install -r requirements.txt
python list_ports.py
copy ..\config\box.windows.json ..\config\box.local.json
# Edit box.local.json — port names must match list_ports output
python sidecar.py --config ..\config\box.local.json
```

Install [loopMIDI](https://www.tobias-erichsen.de/software/loopmidi.html) if you bridge between MPC Software and the sidecar.

## Linux (custom-engine path only)

Same venv steps; use `box.linux.json`. Prefer **PipeWire**; ensure Renaissance appears in `list_ports.py` before starting SP1200 or the sidecar.

## Flags

| Flag | Purpose |
|------|---------|
| `--config PATH` | JSON settings (required) |
| `--log-midi` | Print every message (pad/CC discovery) |
| `--dry-run` | Load config and extensions only |
