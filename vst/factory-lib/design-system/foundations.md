# Foundations

These rules apply to **every theme**. Values live in each theme's `themes/<id>/tokens.json`; the roles every theme must provide, and the minimum quality bar, live in [`contract/roles.json`](contract/roles.json). Theme-specific choices (palette, fonts, brand rules) are explained in each theme's `THEME.md`, e.g. [themes/disklordz/THEME.md](themes/disklordz/THEME.md).

If this document and the JSON disagree, the JSON wins and this file has a bug.

## 1. Principles

1. **Legible first.** Every legend and value is readable at 100 % scale on a laptop in a dark room. Contrast minimums are enforced by `scripts/check_tokens.py`, not reviewed by eye.
2. **Hardware honesty.** Depth means something: **raised** = you can touch it, **recessed (well)** = it shows you something, **flat** = structure. Nothing is raised for decoration.
3. **Every colour has one job.** A theme maps its palette onto roles: `brand` = identity, selection; `active` = on / go; `focus` = keyboard focus; `meter` = signal level; `state.caution` / `state.danger` = careful / wrong; `premium` = paid tier. A colour used outside its role is a bug. Themes declare which primitives are reserved (`usageRestrictions`) and the checker enforces it.
4. **Quiet until touched.** The resting UI is mostly neutral. Colour appears where the user acted (a value arc, a lit LED) or where the audio demands attention (hot, over).
5. **One light, from the top.** Highlights on top edges, shadows below. No other light direction anywhere.
6. **Grid discipline.** 4 px base unit (contract floor). All spacing, sizes and positions are multiples of 4 unless a token says otherwise (stroke widths, radii).

## 2. Colour

### 2.1 Primitives vs roles

- **Primitives** (`color.*`) are a theme's raw palette. Family names are the theme's own business (`purple`, `brand`, `cream`...).
- **Semantic roles** (`semantic.*`) are what components use. **Components never reference a primitive.** Swapping a theme swaps every primitive and remaps the roles; no component code changes.

### 2.2 Semantic roles (required by the contract)

| Group | Tokens | Rule |
|-------|--------|------|
| `bg` | chassis, panel, section, well, overlay | Window, panel and section are distinct steps as they nest. `well` is the recessed surface for readouts. |
| `text` | primary, secondary, tertiary, disabled, onBrand, onActive, onPremium | Primary: legends and live values. Secondary: units, idle values. Tertiary: scale labels and hints, never body copy. |
| `control` | body, bodyTop, rim, track, pointer | Raised control bodies use a `bodyTop → body` gradient (light from the top). |
| `brand` | fill, stroke, pressed, subtle | Value arcs and slider fills use `brand.stroke`. Selected segments use `brand.fill`. |
| `active` | default, hover, pressed, subtle | Only for ON and GO. A knob's value is not "active". |
| `focus` | ring | Keyboard focus only; 2 px, offset 2 px. |
| `state` | ledOff, caution, danger | LEDs and status. Caution = bypass/mute engaged. |
| `premium` | badge | Paid-tier badge only. |
| `meter` | track, rms, peak, hot, over, hold, tick, tickLabel | Level displays only. |

### 2.3 Contrast (contract baseline)

`contract/roles.json → baselineContrast` lists 25 role pairs with minimums: text ≥ 4.5:1 (legends and values ≥ 7:1), non-text indicators ≥ 3:1, plus meter layer separation. A theme may **add or tighten** pairs in its own `contrast` list but can never loosen one.

### 2.4 Restrictions (theme policy, enforced)

`themes/<id>/tokens.json → usageRestrictions` lists which semantic groups may reference a primitive family. Example (DiskLordz): steel cyan only under `semantic.meter`, gold only under `semantic.premium`.

## 3. Typography

Every theme defines two families: **sans** (legends, titles, body, menus) and **mono** (every number: values, readouts, scales).

- **Embedded, not system.** Plugins ship both fonts as JUCE binary data, so Windows and macOS render the same layout. Fonts must be licensed for redistribution (OFL or equivalent); the licence text ships in the release package.
- **Wordmarks are image assets, never UI fonts.** Brand logos live in the theme's assets as vector paths.
- **Numbers are always monospace**, so readouts don't jitter as digits change.
- **Legends are uppercase with positive tracking**, like silkscreen on hardware. Body and tooltip text is sentence case.
- **Minimum size is 10 px** at 100 % scale (contract floor; enforced).

The contract requires these eight styles; each theme sets their font, size, weight, tracking and case:

| Style | Used for |
|-------|----------|
| productName | Header product name |
| sectionTitle | Section panel titles |
| legend | Control labels |
| body | Tooltips, menus, browser rows |
| value | Control value readouts |
| valueLarge | Primary displays |
| scale | Meter and ruler ticks |
| micro | Version, badges, small tags |

**Value formatting.** Use a real minus sign (U+2212) in UI text, not a hyphen. One decimal place for dB (`−12.0 dB`), whole numbers for percent (`62 %`). Use `−∞` at the numeric floor. Units go in `text.secondary`, after a thin space.

## 4. Space and layout

- **Base unit 4 px.** Tokens: `space.1` (4) … `space.16` (64).
- **Window padding** 16. **Header** 40 tall. **Legend gap** 4 (legend ↔ control).
- **Grouping:** related controls sit in a `section` panel (bg.section, radius md, padding 12). Sections are separated by 8 (dense) or 16 (default).
- **Alignment:** controls in a row share a baseline for legends. Knobs of different sizes are centre-aligned on their row.
- **Reading order:** signal flow left → right, top → bottom (input on the left, output on the right). Tab focus follows the same order.

## 5. Shape and depth

Radius values and shadow recipes below are the DiskLordz defaults; other themes set their own in `radius` and `elevation`, but keep the same four elevation levels and their meaning.

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
