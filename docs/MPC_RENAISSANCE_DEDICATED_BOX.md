# MPC Renaissance — dedicated box setup

Operator guide for a **hidden mini PC** + **MPC Renaissance** pseudo-standalone rig. Implementation code lives in [`renaissance-box/`](../renaissance-box/ARCHITECTURE.md).

## What “standalone” means here

The Renaissance is a **USB controller and audio interface**. It does not run MPC projects internally. Standalone feel = **mini PC always attached**, auto-boots into **MPC Software** (and optional **sidecar** tools you control).

You **cannot** flash a new OS onto the Renaissance itself. Custom features belong on the **mini PC** (or in companion scripts), not inside Akai firmware.

## Recommended OS for the dedicated box

| Goal | Mini PC OS | Why |
|------|------------|-----|
| **Official Akai workflow** (pads, plugins, projects, Renaissance driver) | **Windows 10/11** | MPC Software and Renaissance drivers are **Windows/macOS only** — not Linux. |
| **Fully custom app** (no MPC Software) | **Linux** | Run repo **`SP1200_Standalone`** or your own JUCE binary; USB audio/MIDI via class drivers. |
| **Both** | Windows primary + Linux dev/test partition, or second cheap SBC for experiments | Keep production box on Windows for Akai support. |

Your **main Windows machine** stays the dev desk (Cursor, builds, MIDI sniffing). The **box** should be a **Windows mini PC** (N100/N305 class, 16 GB RAM, NVMe) unless you explicitly choose the Linux-only custom-engine path.

## Hardware checklist

- MPC Renaissance + power supply  
- Mini PC mounted behind or under the Renaissance  
- **Short USB cable** (USB 2.0 is fine for MIDI; use a powered port or hub if audio glitches)  
- **Monitor**: optional — use once for setup, then rely on Renaissance LCD + sidecar logging  
- **Keyboard/mouse**: wireless dongle stored inside the enclosure for maintenance  
- Surge protection; label “Renaissance box — do not unplug USB during power-on”

## Recommended path: **1 + 4** (box v1 now, custom engine later)

| Step | When | What |
|------|------|------|
| **1 — Box v1** | First | Windows mini PC + MPC Software only — [BOX_V1_CHECKLIST.md](../renaissance-box/docs/BOX_V1_CHECKLIST.md) |
| **4 — Custom engine** | When you want owned code | Linux (same or second disk) + **SP1200_Standalone** + **Setup → MPC Renaissance map** |

You can keep **v1 on Windows** for daily Akai workflow and add a **Linux boot** (dual-boot or spare SSD) for SP1200 + Renaissance without removing MPC Software from the box.

## Software stack (Mode A — recommended)

1. **Windows** — clean install, auto-login (maintenance account only; see security note below).  
2. **Akai MPC Software** + Renaissance drivers (from Akai installer).  
3. **loopMIDI** — create a port pair, e.g. `RenaissanceSidecar A` / `B`.  
4. **MPC Software MIDI prefs** — Renaissance for performance; optional duplicate/out to loopMIDI for sidecar (or sidecar listens on “Renaissance” port only for tap/sniff in v0).  
5. **Sidecar** — [`renaissance-box/host`](../renaissance-box/host/README.md): custom swing helpers, Disklordz inbox notifications, future LCD bridge.  
6. **Disklordz inbox** (optional) — [`disklordz/daw-inbox`](../disklordz/daw-inbox/ARCHITECTURE.md) on the box or on your dev PC pointing at a shared folder.

### Autostart (Windows)

1. MPC Software → Preferences → **Launch on startup** (if available) or use Startup folder shortcut.  
2. Task Scheduler: run `renaissance-box/scripts/windows/start-sidecar.ps1` at logon (after loopMIDI).  
3. Set Windows audio default device to **Renaissance** outputs/inputs.  
4. Disable sleep; enable **Restart on failure** for sidecar task.

Details: [`renaissance-box/scripts/windows/README.md`](../renaissance-box/scripts/windows/README.md).

## Software stack (Mode B — all custom code)

1. **Linux** (Ubuntu 22.04+ or similar) + **PipeWire**.  
2. Build **`SP1200_Standalone`** from this repo; set JACK/PipeWire device to Renaissance USB.  
3. Open **Setup** view → click **MPC Renaissance map** (pads 36–51, Q-Link CC 1–16, MIDI omni). Re-learn any pad/CC that differs on your unit.  
4. Map pads via SP1200 MIDI learn or [`midi-map.local.json`](../renaissance-box/config/midi-map.template.json).  
5. No MPC Software — you trade Akai project format for full source ownership.

Details: [`renaissance-box/scripts/linux/README.md`](../renaissance-box/scripts/linux/README.md).

## Adding your own features (without breaking Akai)

| Feature type | Where to build | Example |
|--------------|----------------|---------|
| Pad/sequencer logic | Sidecar Python or future JUCE app | Custom swing table, note repeat |
| Sample workflow | Disklordz inbox + sidecar extension | Auto-import new kit folder |
| LCD text | Future SysEx module after capture | Show project name from your app |
| DAW control | Keep on dev PC | Ableton/Bitwig scripts |

**Do not** patch MPC Software binaries. Extend via **MIDI**, **virtual ports**, **watched folders**, and **your own processes**.

## MIDI map verification

Akai maps can depend on firmware and MPC Software mode. Template: [`renaissance-box/config/midi-map.template.json`](../renaissance-box/config/midi-map.template.json).

1. Run `python list_ports.py` on the box.  
2. Run sidecar with `--log-midi` and hit each pad / Q-Link.  
3. Copy template → `midi-map.local.json` and fill verified note/CC numbers.  
4. Point `box.windows.json` at the local map.

## Security (maintenance auto-login)

A box with auto-login and always-on USB is **physically secured** (home studio), not internet-facing. Do not expose RDP to the public internet; keep Windows Update on; use a dedicated local account without admin for daily boot (admin only for installs).

## Troubleshooting

| Symptom | Check |
|---------|--------|
| No pads | Renaissance USB; MPC Software control surface enabled; correct MIDI port in sidecar config |
| Audio crackle | Buffer size in MPC Software; avoid USB hub sharing with heavy storage |
| Sidecar silent | loopMIDI port names match `box.windows.json`; firewall not blocking local MIDI |
| Linux: no Renaissance audio | `pw-cli ls Node`; install firmware if device shows as MIDI-only |

## Next implementation milestones (repo)

1. v0 sidecar + config (this branch)  
2. Verified default MIDI map (community capture on real hardware)  
3. SysEx LCD subset  
4. SP1200 “Renaissance profile” optional UI layout  

Track work in [`renaissance-box/ARCHITECTURE.md`](../renaissance-box/ARCHITECTURE.md).
