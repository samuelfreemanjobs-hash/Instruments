# Night Circuit — preset browser GUI (sound-design + Factory)

**Owner:** Preset sound-design subagent (UX/copy/curation) · **Implementation:** VST Plugin Factory  
**Visual target:** [../design/NightCircuit-GUI-Concept.svg](../design/NightCircuit-GUI-Concept.svg) + [../design/NIGHT_CIRCUIT_GUI_SPEC.md](../design/NIGHT_CIRCUIT_GUI_SPEC.md) (design concept — not a host screenshot).  
**Constraint:** Editor default **1280 × 820**, minimum **1120 × 760** (resizable). VST3 must remain usable in FL Studio and MPC Software plugin windows.

## v0.1 (implemented)

| Element | Purpose |
|---------|---------|
| Category combo | Bass / Synth / Lead / Pad / Pluck/Keys / User |
| Role hint label | One-line workflow guidance per category |
| Search field | Text filter within category |
| **Featured only** toggle | Limits list to `FeaturedPresets.cpp` |
| Preset count label | e.g. `245 Synth presets · 1,028 total` |
| Prev / next | Step within filtered list |
| Star / Favorites | Disk-backed favorites + filter |
| Active preset + Modified | Title row + baseline diff badge |
| Voice mode label | `Poly 16` / `Mono + glide` |
| Category accent stripe | Color coding in header/preset panel |
| User save row | Name + Save to User |

## Visual language (category accents)

| Category | Accent | Hex (UI) |
|----------|--------|----------|
| Bass | Amber | `#e8a035` |
| Synth | Cyan | `#3ecfd4` |
| Lead | Gold | `#f0c040` |
| Pad | Violet | `#9b7ed9` |
| Pluck/Keys | Mint | `#5ecf8a` |
| User | Neutral | `#9090a8` |

## Roadmap (one item per test pass)

Align with [NIGHT_CIRCUIT_GUI_SPEC.md](../design/NIGHT_CIRCUIT_GUI_SPEC.md) increment list (full-width browser, prev/next, panel layout, palette).

1. **Favorites** — user-local star list (does not mutate factory bank).
2. **Featured default** — first open shows featured filter on for Synth/Bass.
3. **Keyboard** — Tab through category → search → preset; Enter to load.
4. **Parameter units** — ms / cents / dB on knobs (separate from browser).
5. **A/B** — compare current vs snapshot (needs processor support).

Spec changes here before implementation; QA runs host + regression pass per item.
