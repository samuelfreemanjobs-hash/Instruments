# Google Stitch UI handoff — JD Upgraded & WAVE-9090

Use **[Google Stitch](https://stitch.withgoogle.com)** (`DeviceType: DESKTOP`) with the prompts and tokens below. Export **HTML/CSS** for engineering reference or **Paste to Figma** for design-system iteration.

## Stitch project setup

| Field | Value |
|-------|--------|
| Project name | `Instruments Plugin UIs` |
| Device | **DESKTOP** |
| Style | Dark DAW / hardware-synth hybrid, high contrast, minimal chrome |
| Audience | Producers loading **500 presets**, bouncing to MPC |

### Design system tokens (create in Stitch first)

```text
Colors
  bg/primary     #0B0B10   (WAVE main)   |  #1A1A22 (JD main)
  bg/panel       #121218   |  #242430 (header stripes)
  bg/elevated    #16161C   |  #2E3A52 (list selection)
  accent/gold    #E8C547   (WAVE knobs)
  text/primary   #F2F2FF   |  #FFFFFF @ 92%
  text/muted     #9AA0B8   |  #B8B8C8
  border/subtle  #303040

Typography
  title          26px bold, "Inter" or "DM Sans"
  section        12–13px bold
  list row       12px regular, mono index prefix

Radii
  panel          8–10px
  control        6px

Spacing grid
  base unit      8px
```

---

## Screen 1 — WAVE-9090

**Default frame:** `940 × 420` (min `820 × 380`, resizable).

### Layout (ASCII)

```text
┌──────────────────────────────────────────────┬─────────────────────┐
│ HEADER: WAVE-9090 + subtitle                 │  PRESET BROWSER     │
│ Analog/Digital Hybrid · 500 presets          │  [<] [>]  17/500   │
├──────────────────────────────────────────────│  Category ▾ Search  │
│ KNOB MATRIX (rotary, gold thumb)             │  ┌─────────────────┐ │
│ Row1: WT Scan Cutoff Res Drive Crush Tape   │  │003 Dark Trap…   │ │
│ Row2: Sub Detune Glide F.Env Width Output   │  │     TRAP        │ │
│                                              │  │ … scroll …     │ │
│ [ Destructive Circuit ] toggle               │  └─────────────────┘ │
└──────────────────────────────────────────────┴─────────────────────┘
```

### Components

| Component | Spec |
|-----------|------|
| Preset browser | 300px right column; list row height **22px**; columns: index (3-digit), name, category right-aligned |
| Category filter | Options: TRAP, PHONK, DRILL, BELL, PAD, LEAD, PLUCK, KEYS, SYNTH, MPC |
| Search | Filters name + category; placeholder `Search presets…` |
| Knobs | 14 rotary, no text box on knob, label under each |
| Destructive Circuit | Toggle, bottom-left of main panel |

### Stitch prompt (copy/paste)

```text
Design a desktop audio plugin UI, 940x420px, dark theme (#0B0B10).

Left 640px: header bar with title "WAVE-9090" and subtitle "500 trap / phonk / MPC factory presets".
Below: two rows of 7 small golden rotary knobs with labels (Wavetable, Scan, Cutoff, Res, Drive, Crush, Tape / Sub, Detune, Glide, F.Env, Width, Output).
Bottom left: toggle "Destructive Circuit".

Right 300px: preset browser panel (#121218). Top row: prev/next chevrons and label "017 / 500 · Night Drive Lead".
Second row: category dropdown "All categories" and search field.
Scrollable list: each row shows "017  Night Drive Lead" left and "TRAP" right; selected row background #2E3A52.

Style: professional DAW, subtle borders #303040, no skeuomorphic wood. High contrast text.
```

---

## Screen 2 — JD Upgraded

**Default frame:** `760 × 920` (resizable up to 1200×1100).

### Layout zones (top → bottom)

1. **Title row** — `JD Upgraded` + ROM source line  
2. **Preset browser** — ~200px height (same component pattern as WAVE-9090)  
3. **Export / Import** buttons (right)  
4. **ROM wave browser** — tone A–D, wave index, mini waveform preview  
5. **FX row** — Master, Res, Grp A/B, Chorus, coupling combo  
6. **4-column tone grid** — level, wave, MS, cutoff, mute, palette per tone  
7. **Envelope section** — amp + filter ADSR, link toggle  

### Category filter options (16)

EP, Pad, Bass, Vaporwave, 80s R&B, 90s R&B, Elite, Trap, Phonk, Lo-Fi, Synthwave, House, Afro, Jersey, Ambient, MPC Pack

### Stitch prompt (copy/paste)

```text
Design a desktop synthesizer plugin UI, 760x920px, charcoal (#1A1A22).

Top: product title "JD Upgraded" and small ROM status text.

Below: large preset browser strip (full width, 200px tall) with search, category filter (16 categories), scrollable 500-preset list showing index, market name (e.g. "Palm Mall Stack"), and category badge (e.g. "Vaporwave"). Summary line "064 / 500 · Palm Mall Stack".

Middle: ROM browser row with tone selector, wave slider, waveform preview canvas, prev/next.

Next: master gain and effect knobs in one row (Master, Res, Grp A, Grp B, Chorus), coupling dropdown.

Bottom: four equal columns for Tone 1–4, each with vertical level, wave/MS rotaries, palette category dropdown, mute M1–M4, colored category accent stripe at top of each column (#category color).

Footer hint text about palette snap and envelopes.

Dark professional JD-800 inspired layout, not a photo-realistic unit. Readable at 100% zoom.
```

---

## Interaction spec (both plugins)

| Interaction | Behavior |
|-------------|----------|
| List click | Loads preset; updates synth parameters |
| Search | Live filter; case insensitive |
| Category | Filters list; "All categories" resets |
| Prev / Next | Moves program ±1 without closing filter |
| Host resize | WAVE: sidebar fixed 300px; JD: browser min 160px height |

---

## Engineering mapping (post-Stitch)

| Stitch element | JUCE implementation |
|----------------|---------------------|
| Preset list | `PresetBrowserComponent` + `ListBox` |
| Knobs | `Slider::RotaryHorizontalVerticalDrag` + APVTS attachments |
| Category combo | `ComboBox` → filter model |
| Colors | `LookAndFeel_V4` (`Wave9090LookAndFeel`) |

Files: `Source/UI/PresetBrowserComponent.*`, `Wave9090/Source/PluginEditor.cpp`, `Source/PluginEditor.cpp`.

---

## Assets to attach in Stitch (optional)

- Screenshot: `/opt/cursor/artifacts/wave9090-preset-browser-ui.png` (reference layout)
- Feature doc: `docs/PRESET_FACTORY_200.md` (update to 500 — see repo)

---

## Export checklist

1. Generate both screens in one Stitch project; apply shared **Design System** tokens above.  
2. Export HTML for each screen → adapt to JUCE (do not paste Tailwind literally).  
3. Paste to Figma if product/marketing needs variants (MPC marketing, pack screenshots).  
4. Name Figma frames: `WAVE-9090 / Main`, `JD Upgraded / Main`.
