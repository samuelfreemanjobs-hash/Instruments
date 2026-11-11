---
name: apc-design
description: APC Design phase — GUI spec for juce-native or webview.
---

# Design phase (`/apc-design [Name]`)

Read `status.json` → `ui_framework`.

## juce-native (default)

Follow **[SOP-VF-02 UI design](../../../docs/disklordz/sops/SOP-VF-02-ui-design.md)**. Start from the template:

```bash
bin/apc template <Name> design
```

Write `plugins/<Name>/.ideas/design-spec.md` using the factory design system (`vst/factory-lib/design-system/`):

- Theme id + version and contract version (record in `status.json → design_system`)
- Window size, exact rects on the 4 px grid, ASCII wireframe
- Control map: every parameter ID → design-system component → tooltip
- States, interaction, tab order, accessibility
- **Colours, type and sizes as role names only — never hex.** A new brand look = a new theme ([SOP-VF-03](../../../docs/disklordz/sops/SOP-VF-03-design-system-theme.md)), not ad-hoc colours.

No C++ required unless user asks for prototype.

## webview (optional / future)

Document HTML canvas regions and JS↔C++ param bridge plan.

## Complete

`validation.design_complete` = true, `current_phase` = `design`

Stop. Next command only: **`/apc-impl <Name>`** (DSP/core logic first, then UI).
