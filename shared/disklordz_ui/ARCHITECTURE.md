# Disklordz UI (`dl_ui`)

Reusable **skinnable** JUCE surface layer for Disklordz plugins (JD Upgraded, SP-1200, …).  
DSP and APVTS stay in each product’s `Source/`; this module only handles layout, LookAndFeel, and assets.

## Goals

- Swap Canva / Google Stitch exports without touching DSP.
- Layout from JSON (control `id`, `x`, `y`, `w`, `h`, `type`).
- Backgrounds and knob filmstrips loaded via `juce_add_binary_data` (PNG; SVG when JUCE path enabled).
- Optional on-screen MIDI keyboard merged in the processor (`MidiKeyboardState`).

## CMake

```cmake
add_subdirectory(shared/disklordz_ui)
target_link_libraries(JDUpgraded PRIVATE dl_ui)
dl_ui_attach_skin(JDUpgraded TARGET JDUpgradedSkinData SKIN jdupgraded_default)
```

## Key types

| Component | Role |
|-----------|------|
| `dl_ui::UiLayout` | Parse `layout.json`, map `id` → bounds / metadata |
| `dl_ui::SkinAssetCache` | Resolve `BinaryData` blobs to `juce::Image` / filmstrips |
| `dl_ui::SkinnableLookAndFeel` | Rotary/filmstrip drawing, fonts, panel fallback |
| `dl_ui::LayoutEditorShell` | Applies layout to a `juce::Component` tree by `componentID` |
| `dl_ui::OnScreenKeyboardStrip` | `MidiKeyboardComponent` + octave shift, velocity ~100 |

See `docs/UI_DESIGN_HANDOFF.md` for designer export checklist.