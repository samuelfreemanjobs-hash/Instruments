# Windows autostart — Renaissance box

## Prerequisites

- loopMIDI installed; create port **RenaissanceSidecar** (or match `outputPortSubstring` in config).
- Sidecar venv at `renaissance-box\host\.venv`.
- `config\box.local.json` copied from `box.windows.json` with correct port substrings.

## Startup folder (MPC Software)

1. Win+R → `shell:startup`
2. Add shortcut to Akai MPC Software (or enable in-app “open at login”).

## Task Scheduler — sidecar

1. Task Scheduler → Create Task.
2. Trigger: **At log on** (box maintenance user).
3. Action: Start program  
   `powershell.exe`  
   Arguments: `-NoProfile -ExecutionPolicy Bypass -File "C:\path\to\repo\renaissance-box\scripts\windows\start-sidecar.ps1"`
4. Conditions: disable “Start only on AC” if using a laptop-class mini PC on battery backup.
5. Settings: restart on failure ×3.

## Audio

Settings → System → Sound → set **Output** and **Input** to MPC Renaissance.

## First boot checklist

- [ ] `python list_ports.py` shows Renaissance input  
- [ ] MPC Software sees Renaissance audio + MIDI  
- [ ] `sidecar.py --dry-run --config ..\config\box.local.json` prints OK  
- [ ] `--log-midi` logs pad hits
