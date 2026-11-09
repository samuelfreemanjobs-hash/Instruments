# DiskLordz Audio Design System — architecture

## Purpose

One visual and interaction language for every DiskLordz Audio plugin UI: colours, type, spacing, depth, motion and component behaviour. Product designers (APC Design phase) pick from it; implementers build it once in `vst/factory-lib/gui/`. Brand anchors come from [docs/DISKLORDZ_BRAND_IDENTITY.md](../../../docs/DISKLORDZ_BRAND_IDENTITY.md).

Status: **v0.1.0 — specification + tokens** (DPCS Phase 1). No C++ yet.

## Build & run

```bash
# validate tokens (exit 0 = pass) and confirm preview.html is current
python3 vst/factory-lib/design-system/scripts/check_tokens.py --check-preview

# contract tests for the checker
python3 -m unittest discover -s vst/factory-lib/design-system/scripts -p "test_*.py"

# after editing tokens.json, regenerate the visual reference
python3 vst/factory-lib/design-system/scripts/render_preview.py
```

Open `preview.html` in a browser to see every token and component.

## Data flow

```text
docs/DISKLORDZ_BRAND_IDENTITY.md   (brand anchors: black, purple, lime, gold)
        │
        ▼
tokens.json  ── single source of truth ──┬──► check_tokens.py   (contrast, restrictions, grid, sizes)
        │                                ├──► render_preview.py ─► preview.html (visual reference)
        │                                └──► [Phase 2] gen_juce_tokens.py ─► gui/DesignTokens.h
        ▼
foundations.md / components.md  (why, when, behaviour)
        │
        ▼
plugins/<Name>/.ideas/design-spec.md  (each product composes components; cites token names, not hex)
```

## Threading / realtime

The design system is UI-only (message thread). Rules for implementers:

- Components never read audio buffers. Meters read atomics published by the processor (see the product's architecture).
- `paint()` must not allocate per frame: cache paths and gradients on `resized()`.
- No design-system header may be included from `Source/Dsp/` or `factory-lib/analysis/`.

## Key modules

| Path | Role |
|------|------|
| `tokens.json` | All values: colour primitives and semantic roles, type, space, radius, stroke, elevation, motion, sizes, interaction, meter constants, contrast pairs, usage restrictions |
| `foundations.md` | Principles, colour roles, typography, layout, depth, motion, scaling, accessibility |
| `components.md` | 13 component specs plus shared state model and input conventions |
| `scripts/check_tokens.py` | Validator. Exit 0 or it isn't done |
| `scripts/test_check_tokens.py` | Contract tests: each breaks one rule and expects failure |
| `scripts/render_preview.py` | Deterministic `preview.html` generator |
| `preview.html` | Generated visual reference (do not edit by hand) |

## Extension points

- **New colour or size:** add it to `tokens.json` (primitive, then semantic role), add contrast pairs for any new foreground/background combination, run the checker, regenerate the preview.
- **Reserved colours:** add a `usageRestrictions` entry; the checker enforces it.
- **New component:** spec it in `components.md` (anatomy, sizes, tokens, states, keyboard, accessibility), add it to the preview, and register its id in `disklordz/vst-factory/modules/gui/controls.json`.
- **Versioning:** semver in `tokens.json`. Patch = value tweak that keeps every contrast pair passing; minor = new tokens or components; major = renamed or removed tokens. Products record the version they were designed against in `plugins/<Name>/status.json → design_system_version`.

## Roadmap

| DPCS phase | Deliverable |
|------------|-------------|
| 1 (this) | tokens.json, foundations, components, validator, preview |
| 2 Contract | `gui/DesignTokens.h` generated from tokens.json (+ freshness check), `gui/DlLookAndFeel.h`, component headers, font binary-data CMake target |
| 3 Module | One component per run: LookAndFeel, RotaryKnob, LedToggle, ValueReadout, LevelMeter, HeaderBar, PresetBrowser |
| 4 Verify | Screenshot tests of each component at 100 % and 200 % scale; checker in CI |

## Related docs

- [foundations.md](foundations.md) · [components.md](components.md) · [preview.html](preview.html)
- [docs/DISKLORDZ_BRAND_IDENTITY.md](../../../docs/DISKLORDZ_BRAND_IDENTITY.md)
- [docs/DiskLordz_Audio_Master_Roadmap_for_Cursor.md §8](../../../docs/DiskLordz_Audio_Master_Roadmap_for_Cursor.md) (visual identity, shared UI system)
- [docs/DPCS.md](../../../docs/DPCS.md) · [docs/dsp-architecture-specification.md](../../../docs/dsp-architecture-specification.md)
