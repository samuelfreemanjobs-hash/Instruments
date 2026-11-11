# SOP-VF-03 — Design-system theme (create, change, version)

## Purpose

Give a product or brand a completely different look **without touching component code**, by supplying a theme that satisfies the role contract.

## Concepts

- **Contract** (`vst/factory-lib/design-system/contract/roles.json`): the 130 role paths every theme must define, floors (10 px text, 24 px targets, 4 px grid), and 25 baseline contrast pairs.
- **Theme** (`themes/<id>/tokens.json` + `THEME.md`): primitives (raw palette) + semantic role mapping + type, space, radius, elevation, motion, sizes, meter constants + policies (`usageRestrictions`, extra `contrast` pairs).
- Themes may **tighten** contract rules, never loosen them.

## Procedure — new theme

1. **Brand inputs:** brand guide hexes, fonts (must be redistributable, e.g. OFL), logo as vector.
2. **Scaffold:** `python3 vst/factory-lib/design-system/scripts/new_theme.py <id> --name "<Display>" --brand "<Brand>"` (copies `_template`; passes immediately).
3. **Primitives:** replace `color.*` with the brand palette. Add tints where the brand colour is too dark or light for thin lines and focus on your surfaces (thin strokes need ≥ 3:1).
4. **Map roles** in `semantic.*`: surfaces (`bg.*`), text, controls, `brand`, `active` (ON/go), `focus`, `state`, `premium`, `meter`. Decide what *each colour means* and never reuse it for a second meaning.
5. **Policies:** add `usageRestrictions` for reserved families (e.g. meter colours only under `semantic.meter`); add stricter `contrast` pairs if the brand wants AAA somewhere.
6. **Type, radius, elevation:** set families and the eight styles; corner radii; shadow recipes (light themes: soft warm shadows; dark themes: strong black).
7. **Check:** `python3 .../check_tokens.py --theme <id>`: fix every `LOW` contrast and every missing role.
8. **Preview:** `python3 .../render_preview.py --theme <id>`; open `previews/<id>.html`; review header, controls, meters at 100 %.
9. **Document:** fill `THEME.md` (use for, brand source, personality, anchors table, additions, policies, type, logo, changelog).
10. **Tests:** `python3 -m unittest discover -s vst/factory-lib/design-system/scripts -p "test_*.py"` (includes "all themes expose identical roles").
11. **PR:** draft PR with the preview screenshot and the contrast report.

## Procedure — change an existing theme

- Value tweak that keeps every pair passing → patch bump (`0.1.0 → 0.1.1`).
- New tokens → minor bump. A role whose *meaning* changes → major bump; update every product `design-spec.md` that depends on it.
- Re-render previews; `check_tokens.py --check-preview` must pass.

## Procedure — new role (contract change)

Only when a component needs something no role covers: add to `contract/roles.json` (minor contract bump), define in **every** theme, add contrast pairs, document in `components.md`/`foundations.md`. The identical-roles test fails until every theme has it.

## Definition of done

```bash
python3 vst/factory-lib/design-system/scripts/check_tokens.py --check-preview   # all themes PASS
python3 -m unittest discover -s vst/factory-lib/design-system/scripts -p "test_*.py"
```

## Related

[design-system ARCHITECTURE](../../../vst/factory-lib/design-system/ARCHITECTURE.md) · [themes/_template/THEME.md](../../../vst/factory-lib/design-system/themes/_template/THEME.md) · [SOP-VF-02](SOP-VF-02-ui-design.md)
