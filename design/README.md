# UI design prototypes

Interactive HTML/CSS mockups for VMPC2000XL. These are **design references** for JUCE implementation (not embedded in the app yet).

## Electribe ESX Hybrid

**Path:** [`electribe-esx/index.html`](electribe-esx/index.html)

Open in a browser:

```bash
cd design/electribe-esx && python3 -m http.server 8765
# http://localhost:8765
```

Or open `index.html` directly.

**Design system:** matte obsidian (`#0c0e13`), walnut rack headers, OLED ice-blue `#38bdf8`, amber downbeats `#f59e0b`, ESX-1 layout (Valve Force, parts matrix, filter, 16-step + motion lanes, touch ribbon).

**Interaction:** Play/Stop transport with step chaser, step toggle (Shift = accent), part select, solo/mute LEDs, draggable knobs, ribbon XY dot, animated VU/tubes.

## Planned variants (spec)

| Variant | Focus |
|---------|--------|
| 1 | Pad Mode & Performance Matrix (4×4, 16 Levels, XY FX) |
| 2 | **Electribe ESX** (this HTML) |
| 3 | Hybrid split-deck (pads left, 16-step right) |

See `docs/UI_VARIANTS.md` for mapping to JUCE `ElectribeModePanel` / future `PadModePanel`.
