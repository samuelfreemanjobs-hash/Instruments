# VST / plug-in hosting

VMPC2000XL is a **hybrid DAW** with a built-in plug-in host (JUCE `AudioPluginFormatManager`).

## Supported formats

| Platform | Formats |
|----------|---------|
| **Linux** | VST3, LV2 |
| **macOS** | Audio Unit (AU), VST3 |
| **Windows** | VST3 |

Classic **VST2** is not enabled (Steinberg SDK licensing). Use VST3 builds of your plug-ins where possible.

## Default scan folders

- **Linux:** `/usr/lib/vst3`, `/usr/local/lib/vst3`, `~/.vst3`, `/usr/lib/lv2`, `~/.lv2`
- **macOS:** `/Library/Audio/Plug-Ins/VST3`, `~/Library/Audio/Plug-Ins/VST3`
- **Windows:** `%ProgramData%\VST3`

Use **Scan VST plug-ins** in the rack at the bottom of the main window after installing new plug-ins.

## Rack behaviour

- **8 slots** — load instruments and/or effects.
- **Instrument** plug-ins (accept MIDI) receive MIDI from the active mode’s sequencer and are summed.
- **Effect** plug-ins process the mixed output in slot order.
- **UI** opens the plug-in’s native editor window.

## Real-time notes

Plug-in load/unload and editor UI run on the **message thread** only. The audio callback processes the current chain with a short try-lock; avoid loading heavy plug-ins during live performance.

## Cache

Discovered plug-ins are cached at:

`~/Library/Application Support/Instruments/VMPC2000XL/known_plugins.xml` (macOS)  
or the platform-equivalent **Application Data** / `Instruments/VMPC2000XL/` folder.
