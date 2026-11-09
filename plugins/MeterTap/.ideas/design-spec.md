# MeterTap — design spec (APC Design)

**UI framework:** `juce-native` 
**Design system:** theme **`disklordz` v0.1.0**, contract **1.0.0** (`vst/factory-lib/design-system/`). All colours, type and sizes below are **role names** from the contract, never hex, so the product can be re-themed by changing one CMake argument (`factory_gui_use_theme(MeterTap THEME disklordz)`). 
**Inputs:** [creative-brief.md](creative-brief.md) · [parameter-spec.md](parameter-spec.md) · [architecture.md](architecture.md) 
**SOP:** [SOP-VF-02 UI design](../../../docs/disklordz/sops/SOP-VF-02-ui-design.md)

Coordinates are logical px at 100 % scale; origin is the window's top-left.

## 1. Window

| Property | Value |
|----------|-------|
| Size | **480 × 440**, fixed (no resize in V1, per brief) |
| Background | `bg.chassis` |
| Padding | `size.windowPadding` (16) |
| UI scale | 100 % in V1. Layout uses tokens only, so the design-system scale steps can be added later without re-layout |
| Keyboard | `EDITOR_WANTS_KEYBOARD_FOCUS TRUE` |

Width rationale: the standard header (glyph, product name, brand name, preset browser ≥ 180 px, version, bypass) needs ≈ 470 px.

## 2. Layout

```text
0                                                                         480
┌──────────────────────────────────────────────────────────────────────────┐ 0
│ [glyph] METERTAP  DISKLORDZ        ‹   Init   ›        v1.0.0  (•)BYPASS │  Header bar (40)
├──────────────────────────────────────────────────────────────────────────┤ 40
│ ┌ METERS · OUTPUT ──────────┐   ┌ INPUT ─────────────────────────────┐   │ 56
│ │  ▬▬        0        ▬▬    │   │              TRIM                  │   │
│ │  ██       −6        ██    │   │             ( ◜◝ )                 │   │
│ │  ██      −12        ██    │   │             +0.0 dB                │   │
│ │  ██                 ██    │   └────────────────────────────────────┘   │ 212
│ │  ██      −24        ██    │   ┌ READOUT ───────────────────────────┐   │ 224
│ │  ██                 ██    │   │        PEAK dBFS     RMS dBFS      │   │
│ │  ██      −36        ██    │   │   L   [  −8.0  ]   [ −15.0  ]      │   │
│ │  ██      −48        ██    │   │   R   [  +1.2  ]   [ −10.0  ]      │   │
│ │  ██      −60        ██    │   │                                    │   │
│ │   L                  R    │   │   (•)HOLD        [ CLEAR ]         │   │
│ └───────────────────────────┘   └────────────────────────────────────┘   │ 424
└──────────────────────────────────────────────────────────────────────────┘ 440
  16                        232   248                                 464
```

### 2.1 Regions

| Region | Rect (x, y, w, h) | Component | Notes |
|--------|-------------------|-----------|-------|
| Header | 0, 0, 480, 40 | HeaderBar | Standard chrome; BYPASS lives here |
| Meter section | 16, 56, 216, 368 | SectionPanel | Title is the meter-mode label (§4) |
| Input section | 248, 56, 216, 156 | SectionPanel | Title `INPUT` |
| Readout section | 248, 224, 216, 200 | SectionPanel | Title `READOUT` |

Section gap 16 (`space.4`), padding 12 (`space.3`). Titles inset 12, 12, `sectionTitle` style, `text.secondary`.

### 2.2 Meter section

| Element | Rect | Component / roles |
|---------|------|-------------------|
| Over cap L | 64, 92, 36, 6 | LevelMeter over cap: `meter.over` latched, `state.ledOff` idle |
| Bar L | 64, 100, 36, 240 | LevelMeter, width `size.meterBar.xl` |
| Scale | 106, 100, 36, 240 | MeterScale: `scale` style, `meter.tickLabel`, `meter.tick` edge marks |
| Over cap R | 148, 92, 36, 6 | as L |
| Bar R | 148, 100, 36, 240 | as L |
| Legends L / R | centred under bars, y 348, h 14 | `legend` style, `text.primary` |

- Range 0 → −60 dBFS, linear in dB (`meter.defaultFloorDb`); `dl_meter_floor_db` fixed at −60, no widget.
- Ticks and labels: `meter.ticksDb`, `meter.labelledTicksDb`.
- Layers: RMS full width (`meter.rms`); peak centre half-width (`meter.peak`, `meter.hot` above `meter.hotThresholdDb`); hold line (`meter.hold`) only when `dl_peak_hold` is on; over cap latched when peak > `meter.overThresholdDb`.
- Click on either bar clears hold and both over latches.

### 2.3 Input section

| Element | Rect | Component / roles |
|---------|------|-------------------|
| Legend `TRIM` | centred on x 356, y 92, h 14 | `legend` |
| Trim knob | ring box 319, 110, 74, 74 (body `size.knob.lg`) | RotaryKnob lg, **bipolar** (arc origin 0 dB) |
| Value | centred on x 356, y 188, h 16 | `value` style, `+0.0 dB` |

### 2.4 Readout section

| Element | Rect | Component / roles |
|---------|------|-------------------|
| Header `PEAK dBFS` | 280, 260, 80, 12 | `micro`, `text.tertiary`, centred |
| Header `RMS dBFS` | 368, 260, 80, 12 | same |
| Row legend `L` | 260, 283, 12, 14 | `legend` |
| PEAK L / RMS L | 280, 276, 80, 28 / 368, 276, 80, 28 | ValueReadout, `valueLarge`, no unit (in header) |
| Row legend `R` | 260, 319, 12, 14 | `legend` |
| PEAK R / RMS R | 280, 312, 80, 28 / 368, 312, 80, 28 | ValueReadout |
| HOLD | 260, 360, 88, 24 | LedToggle md, standard role |
| CLEAR | 356, 360, 96, 24 | Push button sm, secondary |

## 3. Control and display map

| Widget | Binds to | Component | Tooltip |
|--------|----------|-----------|---------|
| Trim | `dl_input_trim_db` (−24…+12 dB, default 0) | RotaryKnob lg, bipolar | **Input trim** · Gain applied before metering and output. · −24…+12 dB · default 0 dB · double-click to reset |
| BYPASS (header) | `dl_bypass` (host bypass parameter) | LedToggle, **caution** role | **Bypass** · Passes audio through untouched. Meters keep showing the input. |
| HOLD | `dl_peak_hold` | LedToggle, standard role | **Peak hold** · Holds the highest peak for 1.5 s, then releases. |
| CLEAR | UI action, no parameter | Push button, secondary | **Clear** · Resets peak hold and over indicators. |
| Meter bars | meter bridge (L, R) via MeterBallistics | LevelMeter xl | **Level** · Peak (narrow bar) and RMS (wide bar) in dBFS. Click to clear. |
| PEAK readouts | held peak (hold on) or display peak (hold off) | ValueReadout | **Peak** · Highest recent sample level per channel. |
| RMS readouts | RMS display value | ValueReadout | **RMS** · Average level over 300 ms. |
| Preset browser | programs (`Init` only) | PresetBrowser | standard |

## 4. States

| State | Visual |
|-------|--------|
| Normal | Meter section title `METERS · OUTPUT`; meters show post-trim signal |
| **Bypassed** | Header BYPASS LED lit (`state.caution`). Meter title becomes `METERS · INPUT` preceded by a 6 px `state.caution` dot. Trim stays editable (automatable); tooltip adds "No effect while bypassed." |
| Silence | Bars empty; readouts `−∞`; over caps idle |
| Hot | Peak bar top in `meter.hot` |
| **Over** | Over cap `meter.over` (latched); PEAK readout in over state (`state.danger`) |
| Hold off | No hold line; PEAK readouts follow display peak |
| Host automating trim | Automation dot at the TRIM legend |

Readout formatting: one decimal, real minus sign, `+` for positive values, `−∞` at or below `meter.numericFloorDb`. Readouts repaint only when their text changes.

## 5. Interaction

Design-system conventions apply (drag 200 px = full range, Shift fine, double-click reset, wheel, click-to-type).

| Control | Specifics |
|---------|-----------|
| Trim | Wheel 0.5 dB; arrows 0.1 dB; Page 3 dB; typed "−6", "-6 dB", "6" accepted |
| HOLD / BYPASS | Space/Enter toggles |
| CLEAR | Space/Enter clears |
| Meters | Click clears; not in tab order (CLEAR covers keyboard users) |

Tab order: preset ‹ → preset name → preset › → BYPASS → Trim → HOLD → CLEAR.

## 6. Accessibility

- Contrast inherited from the theme (contract-checked).
- Over is signalled three ways: cap, readout colour, and screen-reader text "over".
- Screen-reader text: meter "Left: peak −8.0 dB, RMS −15.0 dB"; readouts "Left peak −8.0 decibels"; trim "Input trim, plus 0.0 decibels".
- All targets ≥ 24 px.

## 7. Implementation notes

- Editor = layout + attachments only. All widgets come from `vst/factory-lib/gui/`; theme via `factory_gui_use_theme`.
- 30 Hz timer (`motion.meterRefreshHz`) pulls MeterBridge → MeterBallistics (factory-lib/analysis) → `LevelMeter::setLevels`. Repaint only meters and changed readouts.
- CLEAR and meter clicks reset UI-side hold and over latches only. No parameter writes, no audio-thread calls.
- Re-theme test (Verify): build once with `THEME studio-light`; the editor must compile and render with no code change.
