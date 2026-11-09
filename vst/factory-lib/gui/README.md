# Factory GUI (`factory_gui`)

JUCE LookAndFeel + role-bound components for the [themeable design system](../design-system/ARCHITECTURE.md).

Products pick a theme with **one** CMake argument. Components never hard-code hex colours — they include the generated `DesignTokens.h` for that theme (`fds::color::…` roles).

## CMake

```cmake
# Parent already has JUCE, or configure this folder standalone:
add_subdirectory(vst/factory-lib)   # or path relative to your plugin

target_link_libraries(MyPlugin PRIVATE factory_gui)
factory_gui_use_theme(MyPlugin THEME disklordz)   # or studio-light / _template / custom id
```

`factory_gui_use_theme`:

1. Adds `gui/generated/<theme>/` to the include path (`DesignTokens.h`).
2. Creates a `juce_add_binary_data` target with that theme’s OFL fonts (see `fonts/FONTS.md`).
3. Defines `FACTORY_GUI_THEME_ID`.

Fallback: `-DFACTORY_GUI_USE_SYSTEM_FONTS=ON` skips binary fonts and uses JUCE defaults.

## Editor snippet

```cpp
#include <FactoryGui.h>

class MyEditor : public juce::AudioProcessorEditor
{
public:
    MyEditor (MyProcessor& p)
        : AudioProcessorEditor (p),
          header ("My Product", "1.0.0"),
          drive ("Drive", fds::gui::KnobSize::md)
    {
        setLookAndFeel (&lnf);
        addAndMakeVisible (header);
        addAndMakeVisible (drive);
        setSize (480, 320);
        setWantsKeyboardFocus (true); // focus rings (keyboard only)
    }

    ~MyEditor() override { setLookAndFeel (nullptr); }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (fds::gui::argb (fds::color::bg::panel));
    }

    void resized() override
    {
        auto r = getLocalBounds();
        header.setBounds (r.removeFromTop (fds::size::headerHeight));
        drive.setBounds (r.removeFromLeft (100).removeFromTop (120).reduced (8));
    }

private:
    fds::gui::FactoryLookAndFeel lnf;
    fds::gui::HeaderBar header;
    fds::gui::RotaryKnob drive;
};
```

Set `EDITOR_WANTS_KEYBOARD_FOCUS TRUE` on the plugin for Tab focus + focus rings.

## Regenerate tokens

```bash
python3 vst/factory-lib/design-system/scripts/gen_juce_tokens.py --all
python3 vst/factory-lib/design-system/scripts/check_tokens.py --check-generated
```

## Gallery & render tests

```bash
cmake -B build-factory-gui -S vst/factory-lib \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12 \
  -DCMAKE_BUILD_TYPE=Release -DFACTORY_GUI_THEME=disklordz
cmake --build build-factory-gui -j --target FactoryGuiGallery FactoryGuiRenderTests
ctest --test-dir build-factory-gui -R FactoryGuiRenderTests_ --output-on-failure
# PNGs → build-factory-gui/gui-renders/<theme>/
```

## Components

| Class | Spec |
|-------|------|
| `FactoryLookAndFeel` | rotary / linear / toggle / button / menu / tooltip / combo / label / focus ring |
| `RotaryKnob` | legend + value + bipolar origin + double-click reset + click-to-type |
| `LedToggle` | standard / caution LED roles |
| `ValueReadout` | well, −∞ floor, OVER state |
| `LevelMeter` | display-only `setLevels(peak,rms,hold,over)` — no DSP |
| `MeterScale` | tick column |
| `SectionPanel` | section chrome |
| `HeaderBar` | glyph · product · brand · presets · version · bypass |
| `PresetBrowser` | prev/next/name + popup |
