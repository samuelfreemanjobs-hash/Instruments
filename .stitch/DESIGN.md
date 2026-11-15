---
name: JD Upgraded V-Collection Rack
version: 1.1.0
platform: juce-vst3-desktop
stitch_project: "4458054159206462354"
stitch_screen: "0cd8c1ef7e7d45289902c2bdf827b923"
canvas_px: "1000x760"
colors:
  background: "#0b0e15"
  surface: "#10131a"
  surface_recessed: "#191c22"
  surface_elevated: "#22262f"
  border_subtle: "rgba(255,255,255,0.07)"
  text_primary: "#eceff1"
  text_secondary: "#90a4ae"
  text_muted: "#546e7a"
  text_lcd: "#ffab91"
  accent_primary: "#ff7043"
  primary: "#ff7043"
  semantic_signal: "#00e676"
  semantic_clip: "#ff3d00"
  meter_amber: "#ffca28"
typography:
  font_ui: "Inter, Space Grotesk, system-ui, sans-serif"
  font_telemetry: "JetBrains Mono, Space Grotesk, monospace"
  base_size: "10px"
spacing:
  unit: "4px"
  module_gap: "8px"
  panel_padding: "12px"
rounded:
  chassis: "0px"
  module: "6px"
  control: "4px"
---

# Design System: Anodized Studio Rack VST3

## Overview

A restrained, hardware-inspired **Cockpit Dense** (density 8/10) desktop audio plugin interface modeled on precision studio rack gear. The interface favors crisp modular boundaries, instant scannability, **zero viewport scrolling**, and tactile controls bound to JUCE parameters.

**JD Upgraded (canonical):** Stitch screen [`0cd8c1ef7e7d45289902c2bdf827b923`](https://stitch.google.com/projects/4458054159206462354/screens/0cd8c1ef7e7d45289902c2bdf827b923) — **1000×760** px, Arturia-orange accent `#ff7043`. Full prose: `docs/JD_UPGRADED_V_COLLECTION_STYLE_GUIDE.md`.

**Other Disklordz plugins:** generic **Anodized Studio Rack** tokens may use `#D98838` / 960×600; do not apply JD walnut keybed spec to unrelated products.

**JD Upgraded Hardware v1.0 (Sam):** component library reference at `shared/disklordz_ui/skins/jdupgraded_default/assets/reference/jd-upgraded-component-library-v1.jpg`. Active hardware glow may read **~#FF8C00** on the mockup; token `primary` / `accent_primary` stays **#D98838** until Sam picks one accent for lint + Stitch upload.

## Colors

- **Anodized Obsidian Slate (#141619):** Outer chassis, dark text on engaged amber toggles, frame dividers.
- **Brushed Graphite Panel (#1E2228):** Elevated DSP module cards, preset bar, unengaged control surfaces.
- **Recessed Meter Well (#101215):** Spectrum analyzers, GR meters, numeric readout pills.
- **Hairline Bezel Groove (#2E343D):** 1px module borders, rotary track rings, segmented switch outlines.
- **Crisp Silkscreen White (#F0F2F5):** Active readouts, module headers, preset names.
- **Muted Aluminum Ink (#9BA3B0):** Static knob labels, axis markings, bypassed captions.
- **Warm Studio Amber (#D98838):** Single primary accent (&lt;80% saturation) for value arcs, engaged switches, focus rings.
- **Clean Signal Emerald (#4CA67A):** Normal-headroom meters and signal LEDs only.
- **Hardware Clip Crimson (#F06A6A):** 0 dBFS clip indicators, phase alerts, validation warnings.

Never **#000000** or purple/blue neon glows.

## Typography

- **UI labels:** Inter / system-ui, 11–12px, module titles 13px uppercase at 600 weight, +0.04em tracking.
- **Telemetry:** JetBrains Mono, tabular-nums, 11–12px at 500 weight for dB / Hz / ms / % readouts.
- Labels live in dedicated zones; never overlap waveforms, spectra, or knob caps.

## Layout

- **Viewport:** `overflow: hidden`, no scrollbars; `deviceType: DESKTOP` in Stitch.
- **Header (~44px):** Wordmark, master bypass, preset pill, utility actions.
- **Grid workspace:** 8px gaps, 12px padding; analyzer + knob columns or three-stage rack.
- **Footer (~28px):** Active parameter name, value, CPU, latency.
- **JD Upgraded:** on-screen keyboard strip at bottom per `layout.json` `onScreenKeyboard`.

## Elevation & Depth

Flat hardware stepping via 1px **#2E343D** strokes and surface contrast (#141619 vs #1E2228). No diffused drop shadows or outer neon glows. Active state: solid **#D98838** borders or 1px top specular `rgba(255,255,255,0.06)`.

## Shapes

- **Chassis:** 0px radius.
- **Modules:** 6px (`rounded-md`).
- **Controls / pills:** 4px (`rounded`).
- **Knob caps / LEDs:** `rounded-full`.

## Components

- **Rotary (WebSliderRelay or native filmstrip):** 56×56, 270° arc (#2E343D track, #D98838 value, cap #1E2228, tick #F0F2F5).
- **Fader + meter:** Recessed #101215 track; #4CA67A / #F06A6A LED bars.
- **Toggle (WebToggleButtonRelay):** Segmented; idle #1E2228 / #9BA3B0; active #D98838 on #141619.
- **Combo (WebComboBoxRelay):** Trigger on #101215, border #2E343D.
- **Bypassed analyzer:** Dim grid #2E343D, caption #9BA3B0 — no spinners.

## Do's and Don'ts

**Do:**

- Map every interactive control to a real APVTS id (`WebSliderRelay` / `componentID` + `layout.json`).
- Run `npx -p @google/design.md designmd lint .stitch/DESIGN.md` before `stitch upload design`.

**Don't:**

- Pure black, AI purple gradients, mobile single-column collapse inside desktop plugin frame.
- Overlapping telemetry on meters; marketing hero typography in plugin chrome.