# {{PLUGIN_NAME}} — design spec (APC Design)

**UI framework:** `juce-native` 
**Design system:** theme **`{{THEME}}` v<theme version>**, contract **<contract version>** (`vst/factory-lib/design-system/`). All colours, type and sizes are **role names**, never hex. 
**Inputs:** [creative-brief.md](creative-brief.md) · [parameter-spec.md](parameter-spec.md) · [architecture.md](architecture.md) 
**SOP:** [SOP-VF-02 UI design](../../../docs/disklordz/sops/SOP-VF-02-ui-design.md)

Coordinates are logical px at 100 % scale; origin top-left.

## 1. Window

| Property | Value |
|----------|-------|
| Size | W × H, fixed / resizable |
| Background | `bg.chassis` |
| Padding | `size.windowPadding` |
| UI scale | |
| Keyboard | `EDITOR_WANTS_KEYBOARD_FOCUS TRUE` |

## 2. Layout

```text
(ASCII wireframe with coordinates at the edges)
```

### 2.1 Regions

| Region | Rect (x, y, w, h) | Component | Notes |
|--------|-------------------|-----------|-------|
| Header | 0, 0, W, 40 | HeaderBar | Standard chrome |

### 2.2 <Section> contents

| Element | Rect | Component / roles |
|---------|------|-------------------|
| | | |

## 3. Control and display map

Every parameter in parameter-spec appears here (or is marked "no widget" with a reason).

| Widget | Binds to | Component | Tooltip (name · description · range/default/gesture) |
|--------|----------|-----------|------------------------------------------------------|
| | | | |

## 4. States

| State | Visual (roles) |
|-------|----------------|
| Normal | |
| Bypassed | |
| Disabled / unavailable | |
| Host automating | automation dot |

## 5. Interaction

Design-system conventions apply. Product-specific steps:

| Control | Specifics |
|---------|-----------|
| | |

Tab order: …

## 6. Accessibility

- Screen-reader text per control:
- Colour is never the only signal:
- All targets ≥ 24 px.

## 7. Implementation notes

- Editor = layout + attachments only; widgets from `vst/factory-lib/gui/`; `factory_gui_use_theme({{PLUGIN_NAME}} THEME {{THEME}})`.
- Re-theme test (Verify): build once with another theme; the editor compiles and renders with no code change.
