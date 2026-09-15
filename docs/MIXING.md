# Mixing architecture

## Master rack

Eight slots at the bottom of the main window: instruments, master FX, and Vibe Mixing presets.

## Per-channel strips

Eight **mixer channels** (CH 1–8) sit above the master rack. Each channel has:

- **Fader** and meter (level is saved in the project; per-part level routing comes with multitrack)
- **Ins** — opens two insert slots (internal Resonance or VST FX)

Channel inserts run **after** the master rack on the program bus (serial FX). Per-part multitrack routing is planned; until then, use channels to organize HPF/de-esser/EQ stages on the mix.

## Project save

Mixing state (master + channel plug-ins, parameters, faders, mute) is stored under `<Mixing>` in the project `ValueTree`.

- Auto-save on quit to `Documents/Resonance/Untitled.resonance`
- **Cmd/Ctrl+S** saves the same file
- Internal plug-ins persist parameter values; external VSTs persist binary state + plug-in identifier
