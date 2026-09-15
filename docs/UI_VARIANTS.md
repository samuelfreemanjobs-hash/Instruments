# UI screen variants (product spec)

Three primary surfaces for the VMPC2000XL hybrid DAW. **Variant 2** is prototyped in HTML at `design/electribe-esx/`.

## Variant 1 — Pad Mode & Performance Matrix

- 4×4 velocity pads, banks A–H, sample tags
- 16 Levels, Full/Half Level, Note Repeat (1/16–1/64)
- Left: waveform chop editor, pitch ±36, MPC60 crunch
- Right: XY pad (filter/delay), pad envelope, choke matrix

**JUCE target:** new `PadModePanel` + extend `MaschineModePanel`.

## Variant 2 — Electribe ESX Hybrid (reference built)

- 16-step chaser ribbon, accent/slide/tie/ratchet (HTML: accent via Shift)
- Motion lanes (Pitch, Cutoff, Pan)
- Valve Force tubes, parts matrix (9 drum + 5 synth)
- Filter + mod matrix, arp ribbon

**JUCE target:** evolve `ElectribeModePanel` toward `design/electribe-esx/index.html`.

## Variant 3 — Unified Hybrid Console

- Split: MPC 4×4 left, Electribe 16-step right
- Router: Push Pads → Steps, Bounce Pattern → Pad Chop
- Shared master strip (crunch, ring mod, tubes)

**JUCE target:** dedicated third window or `HybridConsolePanel` replacing single-mode Electribe window.

## Color tokens (shared)

| Token | Hex |
|-------|-----|
| Ice OLED | `#38bdf8` |
| Ice dim | `#0ea5e9` |
| Obsidian | `#0c0e13`, `#111318`, `#1a1b21` |
| Amber accent | `#f59e0b` |
| Walnut trim | `#3d2e24` |
