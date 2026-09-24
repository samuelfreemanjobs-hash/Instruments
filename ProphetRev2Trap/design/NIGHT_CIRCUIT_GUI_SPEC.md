# Night Circuit — GUI specification sheet

**Visual concept (not a host screenshot):** [NightCircuit-GUI-Concept.svg](NightCircuit-GUI-Concept.svg)  
Open in a browser, Figma, or Inkscape. The JUCE editor **does not yet match** this layout pixel-for-pixel; use this doc as the alignment target.

## Product

| Field | Value |
|-------|--------|
| Product | Night Circuit desktop synthesizer plugin |
| Format | **VST3** |
| Target hosts | **FL Studio**, **MPC Software** (desktop) — versions/OS recorded in [../docs/HOST_COMPATIBILITY.md](../docs/HOST_COMPATIBILITY.md) |

## Layout and resize

| Field | Value |
|-------|--------|
| Reference size | **1280 × 820** px |
| Proposed minimum | **1120 × 760** px — verify all controls remain usable at minimum |
| Resize behavior | Proportional **panel** layout; sections stay readable (avoid uniform shrink of all knobs) |
| Current JUCE editor (baseline) | Default **1280 × 820**, minimum **1120 × 760** — see `PluginEditor.cpp` |

## Visual language

| Token | Hex | Use |
|-------|-----|-----|
| Graphite background | `#10101A` | Shell gradient start |
| Panel | `#272236` | Section cards |
| Violet accent | `#B77AEC` | Dials, featured, brand |
| Primary text | `#F5EFFA` | Titles |
| Muted text | `#AA9DBB` | Subtitles, counts |

Category accents in the **shipping** UI may keep role colors (Bass/Synth/…) from [../docs/PRESET_BROWSER_GUI.md](../docs/PRESET_BROWSER_GUI.md) while aligning overall palette to this sheet.

## Preset browser

| Requirement | Detail |
|-------------|--------|
| Navigation | Category dropdown, preset dropdown, **previous / next**, search within category |
| Categories | Bass, Lead, Pad, Pluck/Keys, Synth (+ User bank) |
| Library | **1,028** factory — quotas 195 / 196 / 196 / 196 / 245 |
| Featured | All **73** foundations seeded; sound-design agent expands curated set |
| Favorites | **Persistent per-user** (disk), separate from host project state — *not implemented* |
| Status line | Active preset name; **Modified** when params diverge from loaded preset — *partial* |
| Count copy | e.g. `245 Synth presets · 1,028 total` |
| Voice mode | **Poly 16** / Mono + glide (concept shows combo in browser row) |

### Implementation status (JUCE v0.1)

| Spec item | Status |
|-----------|--------|
| Category + preset + search | Implemented |
| Featured filter | Implemented |
| Prev/next preset | Implemented |
| Favorites + star | Implemented (`FavoritesStore`, disk) |
| Save / Load user (explicit buttons) | Save via “Save to User”; Load via User category |
| Full-width browser band | Implemented |
| Modified indicator | Implemented (APVTS baseline compare) |
| Stitch MCP + DESIGN.md | [../docs/STITCH_MCP.md](../docs/STITCH_MCP.md) |

## Synthesis sections (concept)

| Section | Controls |
|---------|----------|
| Oscillators + filter | Osc mix, detune, shape, sub, cutoff (Hz display), resonance, signed filter env amount |
| Amp envelope | ADSR with **ms / %** labels on knobs |
| Filter envelope | ADSR with unit labels |
| Performance + circuit | Mono glide (inactive in poly), circuit drive |

Map concept labels to existing APVTS where names differ (e.g. **Shape** ↔ pulse/mix DSP — confirm with Factory before rename).

## Accessibility and tech

| Requirement | Detail |
|-------------|--------|
| Controls | Named, keyboard navigable, readable contrast, visible focus |
| Rendering | JUCE-native; no web runtime or external image assets required |
| Host validation | Popup menus, focus, resize, search, preset recall, file dialogs in **FL Studio** and **MPC Software** |

## Roles

| Role | GUI responsibility |
|------|---------------------|
| **vst-gui-designer** | Repo GUI authority: layout, tokens, resize, accessibility, PR review |
| **Sound-design subagent** | Preset copy, featured curation, listening-driven requests |
| **VST Plugin Factory** | JUCE implementation from approved spec |
| **Night Circuit QA** | Host GUI checklist per [../docs/HOST_COMPATIBILITY.md](../docs/HOST_COMPATIBILITY.md) |

## Next implementation increment (recommended order)

1. Full-width **preset browser** row + prev/next + count string from spec.
2. Minimum size **1120 × 760** with panel-based `resized()`.
3. Section cards + palette from concept SVG.
4. **Modified** state + favorites persistence (one PR each with QA pass).
