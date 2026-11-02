# Box v1 checklist (Windows + MPC Software)

Complete this **before** sidecar or custom engines. Goal: power-on → pads and audio work with **only** Akai MPC Software.

## Hardware

- [ ] Renaissance USB to mini PC (direct port, not unpowered hub)
- [ ] Mini PC on UPS or surge protector
- [ ] One-time monitor/keyboard for setup

## Windows

- [ ] Windows 10/11 Pro, fully updated
- [ ] Dedicated local user for daily boot (optional auto-login)
- [ ] **Sleep disabled**; display may sleep
- [ ] Default **playback** and **recording** device: **MPC Renaissance**

## Akai

- [ ] Install latest **MPC Software** + Renaissance driver from Akai
- [ ] Launch MPC Software; confirm **Renaissance** appears as control surface + audio
- [ ] Create empty project; **pad hit** triggers sound (load factory kit if needed)
- [ ] Test **main outs** and **inputs** (incl. phono if used)
- [ ] Save project to local disk; reopen after reboot

## Boot behavior (v1)

- [ ] MPC Software in **Startup** folder or Akai “open at login”
- [ ] No sidecar yet — skip loopMIDI until v2

## Sign-off

When every box above is checked, label the unit **Box v1 OK** with date. Next: optional [sidecar](../host/README.md) (v2) or [Linux + SP1200 custom path](../../docs/MPC_RENAISSANCE_DEDICATED_BOX.md#software-stack-mode-b--all-custom-code) (v4 profile).
