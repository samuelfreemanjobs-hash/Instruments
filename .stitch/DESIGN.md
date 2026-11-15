---
name: Anodized Studio Rack VST3
version: 1.0.0
platform: juce-vst3-desktop
colors:
  background: "#141619"
  surface: "#1E2228"
  surface_recessed: "#101215"
  border_subtle: "#2E343D"
  text_primary: "#F0F2F5"
  text_secondary: "#9BA3B0"
  accent_primary: "#D98838"
  primary: "#D98838"
  semantic_signal: "#4CA67A"
  semantic_clip: "#F06A6A"
typography:
  font_ui: "Inter, system-ui, sans-serif"
  font_telemetry: "JetBrains Mono, monospace"
  base_size: "12px"
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

**Disklordz products:** default DESKTOP frame **960×600** for new WebView UIs; **JD Upgraded** native skin scaffold uses **1020×780** until Sam’s Stitch export lands (see `shared/disklordz_ui/skins/jdupgraded_default/layout.json`).

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