# SOP-VF-10 — Reusable module (factory-lib DSP or GUI block)

## Purpose

Add a block once, in `vst/factory-lib/`, so every later product reuses it instead of re-implementing it.

## When

- A product's architecture needs DSP or UI that isn't in `shared/dsp/` or `vst/factory-lib/`.
- The same code would otherwise appear in two products.

## Procedure

1. **Search first:** `shared/dsp/`, `vst/factory-lib/`, `disklordz/vst-factory/modules/dsp/*.json`, `modules/gui/controls.json`.
2. **Catalog entry:** copy `templates/factory/dsp-block.template.json` (DSP) or `gui-block.template.json` (GUI) into the right catalog file; set id `<category>.<name>`, stub path, `dogfood_sku`.
3. **Contract header (DPCS 2):** from `templates/factory/factory-lib-block.template.h`. DSP blocks are JUCE-free (`<atomic>`, `<array>`, `<cmath>` only) and allocation-free in `process()`. GUI blocks bind to design-system roles only.
4. **GUI blocks:** spec the component in `vst/factory-lib/design-system/components.md` (anatomy, sizes, roles, states, keyboard, accessibility) and add it to the preview generator before writing C++.
5. **Implementation (DPCS 3):** one block per commit with its tests (JUCE-free ctest for DSP; offscreen render tests per theme for GUI).
6. **Docs:** add the block to `vst/factory-lib/README.md` and the DSP knowledge index (`disklordz/vst-factory/knowledge/dsp/index.json`) or the GUI README.

## Definition of done

- Block tests pass; the catalog entry's stub path exists; the dogfood product uses the block (no copy in `Source/`).

## Related

[vst/factory-lib/README.md](../../../vst/factory-lib/README.md) · [SOP-VF-05](SOP-VF-05-contract-implement.md) · [SOP-VF-03](SOP-VF-03-design-system-theme.md)
