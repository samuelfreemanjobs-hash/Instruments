# Factory Design System — architecture

## Purpose

A **themeable template** for plugin UIs. Components are specified once against a **role contract**; a **theme** supplies the look (palette, fonts, radii, shadows) for those roles. Swapping the theme gives a completely different design with no component changes.

- **DiskLordz Audio** products use the `disklordz` theme (brand: [docs/DISKLORDZ_BRAND_IDENTITY.md](../../../docs/DISKLORDZ_BRAND_IDENTITY.md)).
- A new brand or white-label product starts from `_template` with one command.
- `studio-light` is a deliberately different example that proves the swap works.

Status: **v0.2.0 — contract 1.0.0, 3 themes, specification + tooling** (DPCS Phase 1). C++ generation is Phase 2.

## Build & run

```bash
DS=vst/factory-lib/design-system

python3 $DS/scripts/check_tokens.py --check-preview          # all themes: contract + previews fresh (exit 0 = pass)
python3 $DS/scripts/check_tokens.py --theme disklordz        # one theme, with contrast report
python3 -m unittest discover -s $DS/scripts -p "test_*.py"   # contract tests (18)
python3 $DS/scripts/render_preview.py                        # regenerate previews/<theme>.html
python3 $DS/scripts/new_theme.py acme --name "Acme Audio" --brand "Acme"   # new theme from _template
```

Open `previews/<theme>.html` in a browser to see every token and component in that theme.

## Layout

```text
design-system/
├── contract/roles.json        what every theme must provide (130 role paths), floors, 25 baseline contrast pairs
├── themes/
│   ├── _template/             starter: copy this (via new_theme.py), never ship on it
│   ├── disklordz/             DiskLordz Audio brand theme (default for DiskLordz products)
│   └── studio-light/          example: completely different light vintage design
│       ├── tokens.json        values for every role + theme policies (usageRestrictions, extra contrast)
│       └── THEME.md           why: palette rationale, policies, fonts, logo, changelog
├── foundations.md             theme-independent rules: principles, roles, type roles, layout, depth, motion, a11y
├── components.md              13 component specs written against roles only
├── previews/<theme>.html      generated visual reference per theme (do not edit)
└── scripts/                   check_tokens.py · render_preview.py · new_theme.py · test_check_tokens.py
```

## Data flow

```text
brand guide ──► themes/<id>/tokens.json ──┐
                                          ├──► check_tokens.py ──► PASS/FAIL (contract, contrast, policies, floors)
contract/roles.json ──────────────────────┤
                                          ├──► render_preview.py ──► previews/<id>.html
                                          └──► [Phase 2] gen_juce_tokens.py --theme <id> ──► gui/DesignTokens.h
foundations.md + components.md (roles only)
        │
        ▼
plugins/<Name>/.ideas/design-spec.md  → names a theme + version, composes components, cites role names (never hex)
```

## Threading / realtime

UI-only (message thread). Rules for implementers:

- Components never read audio buffers; meters read atomics published by the processor.
- `paint()` must not allocate per frame: cache paths and gradients in `resized()`.
- No design-system header may be included from `Source/Dsp/` or `factory-lib/analysis/`.

## Key modules

| Path | Role |
|------|------|
| `contract/roles.json` | The template's interface: required role paths, floors (10 px text, 24 px targets, 4 px grid), baseline contrast |
| `themes/<id>/tokens.json` | A theme: header (`id`, `name`, `brandName`, `version`, `contract`), primitives, semantic role mapping, type, space, radius, stroke, elevation, motion, size, interaction, meter constants, policies |
| `scripts/check_tokens.py` | Validates every theme against the contract. Themes may tighten, never loosen |
| `scripts/new_theme.py` | Scaffolds a theme from `_template` (or any theme); result passes immediately |
| `scripts/render_preview.py` | Deterministic per-theme preview generator |
| `scripts/test_check_tokens.py` | 18 contract tests incl. "all themes expose identical roles" |

## Extension points

- **New theme / brand:** [SOP-VF-03](../../../docs/disklordz/sops/SOP-VF-03-design-system-theme.md) → `new_theme.py`.
- **New role** (a component needs something no role covers): add it to `contract/roles.json` (minor contract bump), define it in **every** theme, add contrast pairs, update `components.md`. The test "all themes expose identical roles" enforces completeness.
- **New component:** spec it in `components.md` against existing roles, add it to the preview, register its id in `disklordz/vst-factory/modules/gui/controls.json`.
- **Versioning:** contract semver (major = role removed/renamed: every theme must update). Theme semver (patch = value tweak, minor = new tokens, major = role remap that changes meaning). Products record `design_system: {theme, version}` in `plugins/<Name>/status.json`.

## Roadmap

| DPCS phase | Deliverable |
|------------|-------------|
| 1 (this) | contract, 3 themes, foundations, components, validator, previews, scaffolder |
| 2 Contract | `gen_juce_tokens.py --theme <id>` → `gui/DesignTokens.h` (+ freshness check), `gui/DlLookAndFeel.h`, component headers, font binary-data CMake target per theme |
| 3 Module | One component per run: LookAndFeel, RotaryKnob, LedToggle, ValueReadout, LevelMeter, HeaderBar, PresetBrowser |
| 4 Verify | Screenshot tests of each component per theme at 100 % and 200 %; checker in CI |

## Related docs

- [foundations.md](foundations.md) · [components.md](components.md) · [themes/disklordz/THEME.md](themes/disklordz/THEME.md)
- SOPs: [SOP-VF-02 UI design](../../../docs/disklordz/sops/SOP-VF-02-ui-design.md) · [SOP-VF-03 theme](../../../docs/disklordz/sops/SOP-VF-03-design-system-theme.md)
- [docs/DPCS.md](../../../docs/DPCS.md) · [docs/dsp-architecture-specification.md](../../../docs/dsp-architecture-specification.md)
