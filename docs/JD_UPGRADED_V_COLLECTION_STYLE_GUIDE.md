# JD Upgraded — VST/AU Plugin GUI Design System & Interface Style Guide

*Arturia V-Collection Inspired Hardware Synthesizer Interface Architecture*

**Stitch Canvas:** [Project 4458054159206462354](https://stitch.google.com/projects/4458054159206462354) · [Screen `0cd8c1ef7e7d45289902c2bdf827b923`](https://stitch.google.com/projects/4458054159206462354/screens/0cd8c1ef7e7d45289902c2bdf827b923)

---

## 1. Design Philosophy & Vision

The **JD Upgraded** visual language marries the tactile hardware presence of Arturia’s premier virtual instruments (e.g., *Matrix-12 V*, *Prophet-5 V*, *Pigments*, and *Mini V*) with the iconic fader-and-slider hands-on workflow of Roland’s 1991 JD-800 digital synthesizer.

### Core Tenets

1. **Physicality Meets Flat Precision**: Subtle chamfers, inset bezel shadows, knurled metal rotary knobs, and rich walnut wooden cheek-blocks provide physical depth without visual clutter or unnecessary 3D distortion.
2. **Strict Studio Ergonomics**: A modular 12-column grid inside a fixed **1000 × 760 px** footprint ensures 1:1 parity with standard DAW plugin display constraints (Logic Pro, Ableton Live, Pro Tools, Reaper, FL Studio).
3. **High-Contrast Signal Clarity**: Deep slate and dark anodized aluminum chassis (`#10131a`, `#191c22`) contrast sharply with Arturia signature safety orange (`#ff7043` / `#ff5722`), illuminated backlit pushbuttons, and crisp white parameter typography.
4. **Editable Technical Typography**: All parameter values, unit readouts (ms, s, dB, Hz, ST), and status flags remain crisp, high-resolution vector and editable text.

---

## 2. Global Canvas & Mechanical Enclosure Specifications

| Zone | Dimensions (W × H) | Y-Offset | Purpose & Enclosure Details |
| :--- | :--- | :--- | :--- |
| **Plugin Canvas** | 1000 × 760 px | `y = 0` | Master plugin viewport, fixed aspect ratio, dark studio backdrop (`#0b0e15`). |
| **Side End-Cheeks** | 20 px wide (L & R) | Full height | Warm American Walnut grain (`#5d3a1a` to `#2e1708`) with 4 recessed slotted machine screws. |
| **Top Master Bar** | 960 × 72 px | `y = 0` to `y = 72` | Hardware header, recessed LCD patch browser, master gain knob, stereo LED meter, global pitch bend / expression. |
| **Main Control Faceplate** | 960 × 568 px | `y = 72` to `y = 640` | 3-column control bay housing either the 4-Tone Overview or Tone 1–4 Detailed PCM Sound Engine editors. |
| **Damper Felt Strip** | 960 × 4 px | `y = 636` to `y = 640` | Traditional red wool piano keybed felt (`#b71c1c` to `#880e4f`) buffering the faceplate from the keybed. |
| **Keyboard & Wheel Bay** | 960 × 120 px | `y = 640` to `y = 760` | Pitch & Mod wheels + Octave transposer (left 120 px) and 3-octave full-width playable keybed (C2–C5, 840 px). |

---

## 3. Color Palette & Token Architecture

### 3.1 Structural & Chassis Tones

```css
--surface-chassis-bg:        #0b0e15;
--surface-faceplate-dark:    #10131a;
--surface-panel-card:        #191c22;
--surface-panel-elevated:    #22262f;
--surface-panel-highlight:   #2d323e;
--border-hardware-subtle:    rgba(255, 255, 255, 0.07);
--border-hardware-pressed:   rgba(0, 0, 0, 0.45);
--wood-cheek-highlight:      #6a4321;
--wood-cheek-base:           #42240c;
--wood-cheek-shadow:         #1d0d04;
--keybed-felt-red:           #c62828;
```

### 3.2 Accent & Signal Illumination

```css
--accent-arturia-orange:     #ff7043;
--accent-arturia-orange-glow: rgba(255, 112, 67, 0.35);
--accent-amber-warning:      #ffb74d;
--meter-green-nominal:       #00e676;
--meter-amber-warn:            #ffca28;
--meter-red-clip:              #ff3d00;
--active-cyan-soft:            #26c6da;
```

### 3.3 Text & Readout Hierarchies

```css
--text-primary:              #eceff1;
--text-secondary:            #90a4ae;
--text-muted:                #546e7a;
--text-lcd-patch:            #ffab91;
```

---

## 4. Typography Scale & Fonts

The JD Upgraded interface leverages **Space Grotesk** and **Inter** for a surgical blend of industrial synthesizer panel engraving and modern digital clarity.

| Style Role | Font Family | Size | Weight | Tracking | Text Transform |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Plugin Title Wordmark** | Space Grotesk / Inter | 18 px | 800 | +0.12 em | Uppercase |
| **Badge Tag ("UPGRADED")** | Inter | 10 px | 700 | +0.15 em | Uppercase |
| **Section Master Titles** | Space Grotesk / Inter | 12 px | 700 | +0.18 em | Uppercase with accent bullet (●) |
| **Sub-panel Headers** | Inter | 10–11 px | 600 | +0.10 em | Uppercase |
| **Knob / Slider Labels** | Inter | 9–10 px | 600 | +0.06 em | Uppercase |
| **Knob / Slider Values** | Space Grotesk / JetBrains Mono | 10–11 px | 500 | 0.00 em | Regular |
| **Patch Display Text** | Inter | 13 px | 500 | +0.02 em | Title Case |
| **Key Labels (C2, C3...)** | Inter | 8 px | 600 | 0.00 em | Uppercase |

---

## 5. Reusable Component Design Specifications

### 5.1 The Arturia Knurled Rotary Knob

- **Dimensions**: Master Gain 48 px; standard parameter 38 px; micro trim 28 px.
- **Construction**: Graphite bezel, knurled ribbing, brushed anodized cap, 2 px orange radial notch (−135° to +135°), tick marks at 0/25/50/75/100%.

### 5.2 Studio Console Vertical Faders

- **Track**: 4 px width, 80 px (overview) or 50 px (ADSR) height.
- **Fader cap**: 22 × 12 px ribbed aluminum, 2 px orange pip.
- **States**: Active tone — orange glow; muted — charcoal pip, track at 30% opacity.

### 5.3 Illuminated Pushbuttons & Toggles

- **Tone ON/MUTE**: 44 × 20 px; ON `#ff7043`, MUTE `#22262f` with subtle border.
- **Filter slope bar**: 4-segment LPF/BPF/HPF strip with active pill highlight.

### 5.4 Segmented Stereo LED Meter

- 48 px height, 12 segments per channel (green / amber / red zones per §3.2).

### 5.5 Waveform Preview & Vector Scope

- 140 × 60 px well, blueprint grid, 1.5 px trace, monospace header badges.

### 5.6 Piano Keybed & Modulation Section

- 36 keys C2–C5; pitch/mod wheels 22 × 54 px; velocity from mouse Y on key surface.

---

## 6. Layout Grids & Structural Blueprints

```
+-----------------------------------------------------------------------------------+
|  [WOOD] |  TOP BAR: [JD LOGO] [LCD PRESET BROWSER] [MASTER GAIN] [METERS] [BEND] | [WOOD] |
|  CHEEK  +-------------------------------------------------------------------------+  CHEEK |
|         | COLUMN 1 (400px)       | COLUMN 2 (280px)       | COLUMN 3 (240px)      |        |
|         | - 4 Tones Overview     | - Coupling Dropdown    | - Group A (Drive,Phs) |        |
|         |   (Tone 1-4 faders,    | - TVF Global Res Knob  | - Group B (Cho,Dly,Sp)|        |
|         |    waveforms, filters) | - ADSR Envelopes       | - Signal Routing Badge|        |
|         |   [or Detailed Tone 1] |   (TVA & TVF Sliders)  |   (Series A -> B)     |        |
|         +-------------------------------------------------------------------------+        |
|         | RED DAMPER FELT STRIP (960 x 4 px)                                      |        |
|         +-------------------------------------------------------------------------+        |
|         | [PITCH/MOD] [OCT -/0/+] | 3-OCTAVE PIANO KEYBED (C2 to C5, 36 keys)     |        |
+-----------------------------------------------------------------------------------+
```

Map controls to APVTS / `componentID` names in `docs/UI_DESIGN_HANDOFF.md`.

---

## 7. Interaction States & Ergonomic Guidelines

1. **Knobs**: vertical drag; Shift = fine adjust; double-click or Alt+click = default.
2. **Keyboard**: +2 px depression on note-on; velocity from Y on key.
3. **Presets**: stepper arrows; modified patch shows `*` in LCD.
4. **Tone navigation**: tone header → detailed editor; `< OVERVIEW` returns to mixer view.

---

## JUCE implementation notes

| Asset | Path |
|-------|------|
| `layout.json` window | `1000×760` — `shared/disklordz_ui/skins/jdupgraded_default/layout.json` |
| Fonts (BinaryData or system fallback) | Space Grotesk, Inter, JetBrains Mono |
| Stitch upload source | Merge tokens from this file into `.stitch/DESIGN.md` before `stitch upload design` |