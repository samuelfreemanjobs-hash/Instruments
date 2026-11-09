# Components

Each component lists anatomy, sizes, tokens, states, behaviour, keyboard map and accessibility. Components bind to **contract roles only** (`contract/roles.json`), so every spec here works with any theme. Visual reference per theme: `previews/<theme>.html`. Shared interaction values come from the theme's `interaction` tokens.

The factory catalog id (`disklordz/vst-factory/modules/gui/controls.json`) is given where one exists.

## Shared state model

| State | Trigger | Visual rule |
|-------|---------|-------------|
| default | — | As specified per component |
| hover | pointer over | Raised bodies: `bodyTop` → `control.rim` tint; text unchanged |
| pressed / dragging | mouse down | Raised bodies drop their shadow (sit "pushed in"); values switch to `text.primary` |
| focused | keyboard focus | 2 px `focus.ring`, offset 2 px. Never shown on mouse focus |
| disabled | parameter unavailable | Arcs and text → `text.disabled`; no shadow; no hover; still readable |
| automated | host is writing the parameter | Small 4 px `brand.stroke` dot at the legend's right edge. No other change |

## Shared mouse and keyboard conventions

| Input | Effect |
|-------|--------|
| Vertical drag | 200 px = full range (`dragPixelsFullRange`) |
| Shift + drag | Fine: × 0.1 (`fineModifierScale`) |
| Double-click | Reset to default |
| Wheel | 1 % of range per notch (or 1 step for stepped params) |
| Click on value readout | Type a value; Enter commits, Esc cancels; units optional ("−6", "-6 dB") |
| Right-click | Context menu (reserved: MIDI learn, copy/paste value, reset) |
| Arrow up/right · down/left | ± 1 % (or 1 step) |
| Shift + arrow | ± 0.1 % |
| Page up / down | ± 10 % |
| Home / End | Minimum / maximum |
| Space / Enter | Toggle or press (buttons, toggles) |
| Tab / Shift+Tab | Next / previous control in signal-flow order |

Products using keyboard control must set `EDITOR_WANTS_KEYBOARD_FOCUS TRUE` in CMake.

---

## 1. Rotary knob (`gui.rotary-knob`)

**Anatomy:** track ring (`control.track`) · value arc (`brand.stroke`) · raised body (`bodyTop → body` gradient, `control.rim` 1.5 px) · pointer line (`control.pointer`, 2 px, from 35 % to 85 % of the radius) · legend above · value readout below.

| Size | Body Ø | Arc stroke | Use |
|------|--------|-----------|-----|
| sm | 28 | 2 | Dense secondary controls |
| md | 44 | 3 | Default |
| lg | 60 | 3 | Primary controls |
| xl | 84 | 3 | Hero control (one per product, max) |

- **Sweep:** 270°, from 7:30 to 4:30 o'clock (135° → 405° in screen space).
- **Arc origin:** the arc starts at the parameter's **origin**: its minimum for unipolar parameters, its neutral value for bipolar ones (0 dB trim, centre pan). The arc always shows the distance from neutral.
- **Value readout:** `value` style. `text.secondary` at rest, `text.primary` while hovered or dragged.
- **Accessibility:** role slider; title = legend; value text = formatted value with unit.

## 2. Linear slider / fader

**Anatomy:** recessed track (`bg.well`, 4 px thick, radius 2) · fill from origin (`brand.stroke`) · raised thumb 20 × 12 (radius xs).

- Hit area is the full 24 px cross-axis height, not just the 4 px track.
- Vertical faders: same tokens rotated; the legend sits above and the value below.
- Same origin rule, keyboard map and accessibility as the knob.

## 3. LED toggle (on/off parameter)

**Anatomy:** raised rectangular button (height 24 md / 28 lg, radius xs) · 6 px LED at left · uppercase legend.

| Role | LED on | Example |
|------|--------|---------|
| standard | `active.default` + 6 px glow | Hold, Sync, Link |
| caution | `state.caution` + glow | **Bypass**, Mute: states where the product is *not* doing its job |

- LED off = `state.ledOff`, no glow. The legend never changes colour between on and off; the LED carries the state.
- Accessibility: role toggle button; state announced as "on"/"off".

## 4. Push button

| Variant | Body | Text | Use |
|---------|------|------|-----|
| secondary | raised neutral | `text.primary` | Most actions |
| primary | `active.default` fill, `active.pressed` border | `text.onActive` | **One** "go" action per view (Render, Play, Export) |
| brand | `brand.fill` | `text.onBrand` | Selected state inside groups (rare) |

Heights 24 / 28 / 32. Legend style. Pressed state removes the shadow.

## 5. Segmented selector and tabs

- **Segmented** (choice parameter, 2–5 options): connected raised segments; selected = `brand.fill` + `text.onBrand`; others `text.secondary`. Arrow keys move the selection.
- **Tabs** (page navigation, not a parameter): text-only legends; selected = `text.primary` + 2 px `brand.stroke` underline; others `text.secondary`. Divider line `border.divider` under the row.
- More than 5 options → use a dropdown (styled popup menu), not segments.

## 6. Value readout / display

**Anatomy:** recessed well (`bg.well`, radius xs, well elevation) · value in `valueLarge` (or `value`) mono · unit in `micro` style, `text.tertiary`, right-aligned.

| State | Rule |
|-------|------|
| normal | value `text.primary` |
| floor | show `−∞` |
| over / error | value and unit `state.danger`; unit text becomes "OVER" for level readouts |

Click to edit when the readout is bound to a parameter (see conventions).

## 7. Level meter (`gui.meter`)

The one component allowed to use the `meter.*` roles (and the only place `meter.hot` / `meter.over` appear).

**Anatomy (vertical bar):**

| Layer | Token | Geometry |
|-------|-------|----------|
| track | `meter.track` | full bar, well elevation |
| RMS | `meter.rms` | full width, bottom → RMS level |
| peak | `meter.peak` below `hotThresholdDb`, `meter.hot` above | centred, 50 % width, bottom → peak level |
| hold line | `meter.hold` | full width, 2 px, at held peak |
| over cap | `meter.over` when latched, `state.ledOff` otherwise | 6 px tall, 2 px above the bar |

| Bar width | px | Use |
|-----------|----|-----|
| sm | 6 | Inline strips beside controls |
| md | 12 | Channel strips |
| lg | 24 | Main output meters |
| xl | 36 | Metering products (MeterTap) |

- **Scale:** linear in dB from 0 (top) to `defaultFloorDb` (−60, bottom). Ticks at 0, −3, −6, −12, −18, −24, −36, −48, −60; labels at 0, −6, −12, −24, −36, −48, −60. A shared scale column sits between paired bars: `scale` style, `meter.tickLabel` labels, 5 px `meter.tick` marks at both edges.
- **Hot threshold** −6 dBFS; **over** above 0 dBFS. Products may change the range but not the colour logic.
- **Over latch:** the cap stays lit until the user clicks the meter (clears hold and over), or the product resets it.
- **Readout under each bar:** held peak in `value` style; `−∞` at `numericFloorDb` (−96); red when over.
- **Refresh:** 30 Hz from a timer. Ballistics are the product's DSP contract (see each product's architecture), not the widget's.
- **Accessibility:** role meter (read-only); value text "Left peak −8.0 dB, RMS −15.0 dB"; not focusable unless clickable, in which case Space/Enter clears it.

## 8. Header bar (product chrome)

**Layout (40 px tall, `bg.chassis`, `border.divider` bottom line):**

| Left | Centre | Right |
|------|--------|-------|
| brand glyph (22 px vector) · product name (`productName`) · "DiskLordz" (`micro`, tertiary) | preset browser | version (`micro`) · global Bypass (caution LED toggle) |

Every DiskLordz plugin uses this header unchanged, so users always know where presets and bypass are.

## 9. Preset browser

- Recessed well (min width 180, height 24) with ‹ › arrows (24 × 24 hit targets) and the preset name centred.
- Click the name → popup menu: factory presets, user presets, then Save, Save as…, Rename, Delete, Open folder.
- Modified preset: name suffixed with " *" in `text.secondary`.
- Arrows wrap around. Loading happens off the audio thread.

## 10. Tooltip

- `bg.overlay`, 1 px `border.overlay`, radius sm, overlay elevation, max width 240, padding 8 × 10, `body` style.
- Content: **name** (bold) · one-line description · tertiary line with range, default and gesture hint.
- 600 ms delay, 100 ms fade. Every interactive control has a tooltip.

## 11. Section panel

- `bg.section`, radius md, padding 12. Optional title in `sectionTitle` style, `text.secondary`, inset 12 from the top-left.
- Groups controls that belong to the same stage of the signal flow. Don't nest sections.

## 12. Popup menu

- `bg.overlay`, overlay elevation, radius sm, `body` style rows 24 px tall.
- Hover row: `brand.subtle` background. Checked item: 6 px `active.default` LED in the left gutter. Separators `border.subtle`. Disabled rows `text.disabled`.

## 13. Premium badge

`premium.badge` fill, `text.onPremium`, `micro` style, radius xs, padding 2 × 6. Used only to mark paid tiers or premium features. Never as a button.
