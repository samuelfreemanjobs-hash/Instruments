# Linux — custom-engine box (no MPC Software)

Use when the dedicated box runs **Renaissance Host** (JUCE) instead of Akai MPC Software.

## PipeWire

```bash
pw-cli ls Node | grep -i akai
```

Set default source/sink to Renaissance in `wpctl status` / `pavucontrol`.

## systemd user unit (example)

Create `~/.config/systemd/user/renaissance-sidecar.service`:

```ini
[Unit]
Description=Renaissance MIDI sidecar
After=pipewire.service

[Service]
WorkingDirectory=/home/studio/instruments/renaissance-box/host
ExecStart=/home/studio/instruments/renaissance-box/host/.venv/bin/python sidecar.py --config /home/studio/instruments/renaissance-box/config/box.linux.json
Restart=on-failure

[Install]
WantedBy=default.target
```

```bash
systemctl --user enable --now renaissance-sidecar.service
```

## JUCE host autostart

Point `ExecStart` at `build/renaissance-box/juce-host/RenaissanceHost_artefacts/Release/Standalone/Renaissance Host` after building from repo root.
