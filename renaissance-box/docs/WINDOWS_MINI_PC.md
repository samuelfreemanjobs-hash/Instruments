# Windows mini PC — Renaissance box

**Your dedicated box should be a Windows mini PC** (N100 / N305 class). The Renaissance needs a USB host; Akai **MPC Software and drivers are Windows-only**, and the JUCE **Renaissance Host** builds natively here too.

## Suggested hardware

| Part | Notes |
|------|--------|
| Mini PC | 16 GB RAM, 256 GB+ NVMe, Intel N100/N305 or similar |
| USB | Short cable, PC rear port preferred |
| UPS | Recommended for studio power bumps |
| Monitor/KB | Only for initial setup; box can run headless after v1 |

## Phase 1 — Box v1 (Akai only)

Follow [BOX_V1_CHECKLIST.md](BOX_V1_CHECKLIST.md):

1. Clean **Windows 11** (or 10), updates, **no sleep**
2. Install **MPC Software** + Renaissance driver from Akai
3. **Settings → Sound** → output **and** input = **MPC Renaissance**
4. MPC Software in **Startup** (`Win+R` → `shell:startup`)
5. Confirm pads + audio + project save across reboot

Do **not** install loopMIDI or the Python sidecar until v1 passes.

## Phase 2 — JUCE Renaissance Host (your features)

On the **same** mini PC (after v1):

### Build tools

- [Visual Studio 2022](https://visualstudio.microsoft.com/) with **Desktop development with C++**
- [CMake](https://cmake.org/download/) 3.22+
- Clone this repo, e.g. `C:\instruments`

### Build

```powershell
cd C:\instruments
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target RenaissanceHost_Standalone
```

Binary (typical):

`build\renaissance-box\juce-host\RenaissanceHost_artefacts\Release\Standalone\Renaissance Host.exe`

### Run

1. Quit MPC Software (only one app should own Renaissance audio at a time).
2. Confirm default audio device is still **MPC Renaissance**.
3. Launch **Renaissance Host.exe** — pads should trigger tones; Q-Links adjust levels.
4. Optional: Startup shortcut to Renaissance Host **instead of** MPC Software when you live on custom code daily.

Architecture: [JUCE_RENAISSANCE_HOST.md](../../docs/JUCE_RENAISSANCE_HOST.md).

### MIDI verify

If pads are silent, use optional Python sniff (on dev PC or box):

```powershell
cd C:\instruments\renaissance-box\host
python -m venv .venv
.\.venv\Scripts\activate
pip install -r requirements.txt
python list_ports.py
python sidecar.py --config ..\config\box.windows.json --log-midi
```

Adjust `RenaissanceSurfaceMap` in JUCE if note/CC numbers differ.

## Autostart (custom host)

1. `Win+R` → `shell:startup`
2. Shortcut to `Renaissance Host.exe`
3. Remove or disable MPC Software startup shortcut when switching permanently

## Linux on the mini PC?

**Not required.** Linux is optional for experiments; production **pseudo-standalone with MPC Software** = Windows only.
