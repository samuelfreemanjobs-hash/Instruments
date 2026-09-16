---
name: apc-design
description: APC Design phase — GUI spec for juce-native or webview.
---

# Design phase (`/apc-design [Name]`)

Read `status.json` → `ui_framework`.

## juce-native (default)

Write `plugins/<Name>/.ideas/design-spec.md`:

- Window size
- Control layout (grid)
- Label text per parameter ID
- Color palette (hex)

No C++ required unless user asks for prototype.

## webview (optional / future)

Document HTML canvas regions and JS↔C++ param bridge plan.

## Complete

`validation.design_complete` = true, `current_phase` = `design`

Stop: `/apc-impl <Name>`
