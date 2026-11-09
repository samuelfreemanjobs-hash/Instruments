# Foundations

Values live in [`tokens.json`](tokens.json). This document explains **why** and **when**. If the two disagree, the JSON wins and this file has a bug.

## 1. Principles

1. **Legible first.** Every legend and value is readable at 100 % scale on a laptop in a dark room. Contrast minimums are enforced by `scripts/check_tokens.py`, not reviewed by eye.
2. **Hardware honesty.** Depth means something: **raised** = you can touch it, **recessed (well)** = it shows you something, **flat** = structure. Nothing is raised for decoration.
3. **Every colour has one job.** Purple says *this is DiskLordz / this is selected / this has focus*. Lime says *on / go*. Cyan says *signal level*. Amber and red say *careful* and *wrong*. A colour used outside its job is a bug (enforced for cyan, gold, amber and red).
4. **Quiet until touched.** The resting UI is mostly neutral. Colour appears where the user acted (a value arc, a lit LED) or where the audio demands attention (hot, over).
5. **One light, from the top.** Highlights on top edges, shadows below. No other light direction anywhere.
6. **Grid discipline.** 4 px base unit. All spacing, sizes and positions are multiples of 4 unless a token says otherwise (stroke widths, radii).

## 2. Colour

### 2.1 Brand anchors (from `docs/DISKLORDZ_BRAND_IDENTITY.md`)

| Brand role | Hex | Token | UI job |
|------------|-----|-------|--------|
| Black | `#0A0A0C` | `color.neutral.950` → `bg.chassis` | Window background |
| Purple | `#7B2CBF` | `color.purple.600` → `brand.fill` | Selected fills (segment, tab, preset row), header accents |
| Accent lime | `#CDFF00` | `color.lime.500` → `active.default` | ON state (LEDs), primary "go" button |
| Metallic gold | `#C5A059` | `color.gold.500` → `premium.badge` | Premium badge / crown only |
| Burgundy | `#631C1E` | — | Marketing only; not a UI token |

### 2.2 Additions in this system

| Family | Why it exists |
|--------|---------------|
| **Neutral 1000…0** | A cool gunmetal ramp built down from brand black: panels, controls, text. |
| **Purple 900/700/400/300** | `#7B2CBF` is too dark for thin lines on black (2.5:1). `purple.400` is the stroke tint (≥ 3:1 on tracks and panels); `purple.300` is the focus ring. |
| **Lime 900/700/300** | Hover, pressed and tint variants of the brand accent. |
| **Cyan 700/400** | **Meters and level displays only** (operator decision, v0.1). Keeps signal information visually separate from brand and state colour. |
| **Signal amber / red** | Hot zone, caution states (bypass engaged), over and error. Distinct from gold and burgundy on purpose. |

### 2.3 Semantic roles

Components use **semantic** tokens only (`semantic.*`), never primitives. Primitives exist so semantic roles can be retuned in one place.

| Group | Tokens | Rule |
|-------|--------|------|
| `bg` | chassis, panel, section, well, overlay | Window → panel → section get lighter as they nest. `well` is the darkest: recessed displays and meters. |
| `text` | primary, secondary, tertiary, disabled, onBrand, onActive, onPremium | Primary: legends and live values. Secondary: units, idle values. Tertiary: scale labels and hints, never body copy. |
| `control` | body, bodyTop, rim, track, pointer | Raised control bodies use a `bodyTop → body` gradient (light from the top). |
| `brand` | fill, stroke, pressed, subtle | Value arcs and slider fills use `brand.stroke`. |
| `active` | default, hover, pressed, subtle | Only for ON and GO. A knob's value is not "active". |
| `focus` | ring | Keyboard focus only; 2 px, offset 2 px. |
| `state` | ledOff, caution, danger | LEDs and status. |
| `meter` | track, rms, peak, hot, over, hold, tick, tickLabel | Level displays only. |

### 2.4 Restrictions (enforced)

`tokens.json → usageRestrictions` lists which semantic groups may reference a primitive. `check_tokens.py` fails if, for example, a knob arc points at cyan or a button uses gold.

## 3. Typography

| Family | Font | Licence | Use |
|--------|------|---------|-----|
| sans | **Inter** | OFL-1.1 | Legends, titles, body, menus |
| mono | **JetBrains Mono** | OFL-1.1 | Every number: values, readouts, scales |

- **Embedded, not system.** Plugins ship both fonts as JUCE binary data. System UI fonts differ between Windows and macOS (Segoe UI vs SF), which breaks fixed hardware-style layouts. The licence text ships in the release package.
- **The blackletter DiskLordz wordmark is an image asset, never a UI font** (brand v1). In plugin chrome use the simplified floppy-and-crown glyph.
- **Numbers are always monospace**, so readouts don't jitter as digits change.
- **Legends are uppercase with +8 % tracking**, like silkscreen on hardware. Body and tooltip text is sentence case.
- **Minimum size is 10 px** at 100 % scale (enforced).

| Style | Size / line | Weight | Case | Used for |
|-------|-------------|--------|------|----------|
| productName | 18 / 22 | 700 | upper | Header product name |
| sectionTitle | 12 / 16 | 600 | upper | Section panel titles |
| legend | 11 / 14 | 600 | upper | Control labels |
| body | 12 / 16 | 400 | sentence | Tooltips, menus, browser rows |
| value | 12 / 16 mono | 500 | as-is | Control value readouts |
| valueLarge | 16 / 20 mono | 500 | as-is | Primary displays |
| scale | 10 / 12 mono | 500 | as-is | Meter and ruler ticks |
| micro | 10 / 12 | 600 | upper | Version, badges, small tags |

**Value formatting.** Use a real minus sign (U+2212) in UI text, not a hyphen. One decimal place for dB (`−12.0 dB`), whole numbers for percent (`62 %`). Use `−∞` at the numeric floor. Units go in `text.secondary`, after a thin space.

## 4. Space and layout

- **Base unit 4 px.** Tokens: `space.1` (4) … `space.16` (64).
- **Window padding** 16. **Header** 40 tall. **Legend gap** 4 (legend ↔ control).
- **Grouping:** related controls sit in a `section` panel (bg.section, radius md, padding 12). Sections are separated by 8 (dense) or 16 (default).
- **Alignment:** controls in a row share a baseline for legends. Knobs of different sizes are centre-aligned on their row.
- **Reading order:** signal flow left → right, top → bottom (input on the left, output on the right). Tab focus follows the same order.

## 5. Shape and depth

| Token | Value | Use |
|-------|-------|-----|
| radius.xs | 2 | Buttons, toggles, readout wells |
| radius.sm | 3 | Chips, tooltips |
| radius.md | 4 | Section panels |
| radius.pill | 999 | LEDs only |

Hardware edges are tight: nothing larger than 4 px except round controls.

| Elevation | Recipe | Use |
|-----------|--------|-----|
| **well** | inner shadow (black 70 %, y 1, blur 2) + bottom highlight (white 5 %) | Readouts, meters, preset display |
| **flat** | none | Panels, sections |
| **raised** | `bodyTop → body` gradient + top highlight (white 7 %) + drop shadow (black 55 %, y 2, blur 4) | Knobs, buttons, toggles, slider thumbs |
| **overlay** | drop shadow (black 60 %, y 6, blur 16) + 1 px border | Tooltips, menus |

No photographic textures, brushed metal or noise in v0.1. Depth comes from the recipes above only.

## 6. Motion

| Token | Value | Use |
|-------|-------|-----|
| hoverMs | 80 | Hover tint in/out |
| pressMs | 60 | Press feedback |
| toggleMs | 120 | LED and segment changes |
| menuMs | 120 | Menu open |
| tooltipDelayMs / FadeMs | 600 / 100 | Tooltip appear |
| meterRefreshHz | 30 | Meter repaint rate |

- Easing: ease-out cubic for everything that animates.
- **Value changes are never animated.** A knob follows the mouse or host automation exactly.
- **Reduced motion:** a global user setting sets all UI transitions to 0 ms. Meters keep their ballistics, because the ballistics are information, not decoration.

## 7. Scaling

- All tokens are logical pixels at 100 %. Supported UI scales: 75, 100, 125, 150, 200 % (`size.scaleSteps`).
- **Vector drawing only.** No raster UI images, so every scale is sharp. The brand glyph ships as a vector path.
- Each product sets a minimum window size; text must stay ≥ 10 px at 100 %.

## 8. Accessibility

| Requirement | How it's met |
|-------------|--------------|
| Text contrast ≥ 4.5:1 (legends/values ≥ 7:1) | `contrast` pairs in tokens.json, enforced |
| Non-text contrast ≥ 3:1 (arcs, LEDs, focus, meter fills) | Same, enforced |
| Colour is never the only signal | Over = red **and** a cap shape **and** "OVER" readout; LEDs differ by glow as well as colour; selected tab = underline + text weight |
| Target size ≥ 24 × 24 px | `size.hitTargetMin`, enforced for knobs, buttons, toggles |
| Keyboard operable | Every control is focusable; see component keyboard maps |
| Screen readers | Every control exposes title, value text and role via JUCE accessibility handlers |
| Focus visible | 2 px `focus.ring`, offset 2 px, on keyboard focus only (not on mouse click) |
