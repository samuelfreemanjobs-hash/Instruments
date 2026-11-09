# Factory Design System — architecture

## Purpose

A **themeable template** for plugin UIs. Components are specified once against a **role contract**; a **theme** supplies the look (palette, fonts, radii, shadows) for those roles. Swapping the theme gives a completely different design with no component changes.

- **DiskLordz Audio** products use the `disklordz` theme (brand: [docs/DISKLORDZ_BRAND_IDENTITY.md](../../../docs/DISKLORDZ_BRAND_IDENTITY.md)).
- A new brand or white-label product starts from `_template` with one command.
- `studio-light` is a deliberately different example that proves the swap works.

Status: **v0.3.0 — contract 1.0.0, 3 themes, JUCE tokens + LookAndFeel + components + render tests** (DPCS Phases 1–4).

## Build & run

```bash
DS=vst/factory-lib/design-system

python3 $DS/scripts/check_tokens.py --check-preview --check-generated  # contract + previews + DesignTokens.h fresh
python3 $DS/scripts/check_tokens.py --theme disklordz                  # one theme, with contrast report
python3 $DS/scripts/gen_juce_tokens.py --all                           # → gui/generated/<id>/DesignTokens.h
python3 $DS/scripts/gen_juce_tokens.py --all --check                   # freshness only
python3 -m unittest discover -s $DS/scripts -p "test_*.py"             # contract + generator tests
python3 $DS/scripts/render_preview.py                                  # regenerate previews/<theme>.html
python3 $DS/scripts/new_theme.py acme --name "Acme Audio" --brand "Acme"

# JUCE gallery + offscreen render tests (Linux: g++-12)
cmake -B build-factory-gui -S vst/factory-lib -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12 -DCMAKE_BUILD_TYPE=Release
cmake --build build-factory-gui -j --target FactoryGuiGallery FactoryGuiRenderTests
ctest --test-dir build-factory-gui -R FactoryGuiRenderTests_ --output-on-failure
```

Open `previews/<theme>.html` in a browser to see every token and component in that theme. Product CMake: see [`gui/README.md`](../gui/README.md).

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
├── components.md              13 component specs written against roles only (+ JUCE class names)
├── previews/<theme>.html      generated visual reference per theme (do not edit)
└── scripts/                   check_tokens · gen_juce_tokens · render_preview · new_theme · tests

../gui/
├── generated/<theme>/DesignTokens.h   header-only constexpr tokens (no JUCE include)
├── FactoryLookAndFeel.*               LookAndFeel_V4 bound to roles
├── components/                        RotaryKnob, LedToggle, ValueReadout, LevelMeter, …
├── fonts/<Family>/                    vendored OFL TTFs + FONTS.md
├── gallery/                           FactoryGuiGallery standalone
└── tests/                             FactoryGuiRenderTests (100% / 200% PNGs)
```

## Data flow

```text
brand guide ──► themes/<id>/tokens.json ──┐
                                          ├──► check_tokens.py ──► PASS/FAIL (contract, contrast, policies, floors)
contract/roles.json ──────────────────────┤
                                          ├──► render_preview.py ──► previews/<id>.html
                                          └──► gen_juce_tokens.py --theme <id> ──► gui/generated/<id>/DesignTokens.h
foundations.md + components.md (roles only)
        │
        ▼
factory_gui_use_theme(<target> THEME <id>)  → include path + font binary data
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
| `scripts/check_tokens.py` | Validates every theme against the contract. Themes may tighten, never loosen. `--check-generated` freshness |
| `scripts/gen_juce_tokens.py` | Emits `gui/generated/<id>/DesignTokens.h` (`fds::` constexpr); `--check` freshness |
| `scripts/new_theme.py` | Scaffolds a theme from `_template` (or any theme); result passes immediately |
| `scripts/render_preview.py` | Deterministic per-theme preview generator |
| `scripts/test_check_tokens.py` / `test_gen_juce_tokens.py` | Contract + generator unit tests |
| `../gui/` + `../CMakeLists.txt` | `factory_gui` INTERFACE, `factory_gui_use_theme`, gallery, render tests |
| `.github/workflows/design-system.yml` | Checker + unittest + Linux build/ctest on `vst/factory-lib/**` |

## Extension points

- **New theme / brand:** [SOP-VF-03](../../../docs/disklordz/sops/SOP-VF-03-design-system-theme.md) → `new_theme.py`.
- **New role** (a component needs something no role covers): add it to `contract/roles.json` (minor contract bump), define it in **every** theme, add contrast pairs, update `components.md`. The test "all themes expose identical roles" enforces completeness.
- **New component:** spec it in `components.md` against existing roles, add it to the preview, register its id in `disklordz/vst-factory/modules/gui/controls.json`.
- **Versioning:** contract semver (major = role removed/renamed: every theme must update). Theme semver (patch = value tweak, minor = new tokens, major = role remap that changes meaning). Products record `design_system: {theme, version}` in `plugins/<Name>/status.json`.

## Roadmap

| DPCS phase | Deliverable | Status |
|------------|-------------|--------|
| 1 | contract, 3 themes, foundations, components, validator, previews, scaffolder | **done** (v0.2) |
| 2 Contract | `gen_juce_tokens.py` → `gui/generated/<id>/DesignTokens.h` + `--check` / `--check-generated`; CMake `factory_gui` + `factory_gui_use_theme`; OFL fonts | **done** (v0.3) |
| 3 Module | `FactoryLookAndFeel`, `RotaryKnob`, `LedToggle`, `ValueReadout`, `LevelMeter`, `MeterScale`, `SectionPanel`, `HeaderBar`, `PresetBrowser` | **done** (v0.3) |
| 4 Verify | `FactoryGuiRenderTests` per theme at 100 % / 200 %; PNGs under `build/gui-renders/`; CI workflow | **done** (v0.3) |

## Related docs

- [foundations.md](foundations.md) · [components.md](components.md) · [themes/disklordz/THEME.md](themes/disklordz/THEME.md)
- SOPs: [SOP-VF-02 UI design](../../../docs/disklordz/sops/SOP-VF-02-ui-design.md) · [SOP-VF-03 theme](../../../docs/disklordz/sops/SOP-VF-03-design-system-theme.md)
- [docs/DPCS.md](../../../docs/DPCS.md) · [docs/dsp-architecture-specification.md](../../../docs/dsp-architecture-specification.md)
