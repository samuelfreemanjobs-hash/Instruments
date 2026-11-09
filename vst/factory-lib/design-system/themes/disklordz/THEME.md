# DiskLordz Audio theme (`disklordz`)

**Use for:** every DiskLordz Audio product (default theme). 
**Brand source:** [docs/DISKLORDZ_BRAND_IDENTITY.md](../../../../../docs/DISKLORDZ_BRAND_IDENTITY.md) 
**Personality:** studio hardware: machined dark panels, subtle depth, precise silkscreen legends. 
**Preview:** [previews/disklordz.html](../../previews/disklordz.html)

## Brand anchors

| Brand role | Hex | Primitive → role | UI job |
|------------|-----|------------------|--------|
| Black | `#0A0A0C` | `color.neutral.950` → `bg.chassis` | Window background |
| Purple | `#7B2CBF` | `color.purple.600` → `brand.fill` | Selected fills (segment, tab, preset row), header accents |
| Accent lime | `#CDFF00` | `color.lime.500` → `active.default` | ON state (LEDs), primary "go" button |
| Metallic gold | `#C5A059` | `color.gold.500` → `premium.badge` | Premium badge / crown only |
| Burgundy | `#631C1E` | — | Marketing only; not a UI token |

## Additions beyond the brand guide

| Family | Why |
|--------|-----|
| Neutral 1000…0 | Cool gunmetal ramp built down from brand black: panels, controls, text |
| Purple 900/700/400/300 | `#7B2CBF` is only 2.5:1 on black, too dark for thin lines. `purple.400` = arcs and strokes (≥ 3:1), `purple.300` = focus ring |
| Lime 900/700/300 | Hover, pressed and tint variants of the accent |
| **Steel cyan 700/400** | **Meters and level displays only** (operator decision, 2026-11-08). Keeps signal information separate from brand and state colour |
| Signal amber / red | Hot, caution (bypass engaged), over, error. Deliberately distinct from gold and burgundy |

## Policies (enforced by the checker)

- `color.cyan` only under `semantic.meter`.
- `color.gold` only under `semantic.premium`.
- `color.signal` only under `semantic.meter` and `semantic.state`.
- Text on lime stays ≥ 7:1 (stricter than the contract's 4.5:1).

## Type

Inter (sans) and JetBrains Mono (mono), both OFL-1.1, embedded. Legends uppercase, +8 % tracking. Product name 18/22 bold uppercase.

## Logo in plugin chrome

Simplified floppy-and-crown glyph as a vector path (brand v1: never the full circular crest in small UIs; never the blackletter wordmark as a font).

## Changelog

| Version | Change |
|---------|--------|
| 0.1.0 | Initial theme (extracted from design system v0.1.0 when it became themeable) |
