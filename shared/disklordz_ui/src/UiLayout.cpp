#include "dl_ui/UiLayout.h"

namespace dl_ui
{

namespace
{

ControlKind kindFromString (const juce::String& s)
{
    if (s == "background") return ControlKind::background;
    if (s == "knob") return ControlKind::knob;
    if (s == "slider") return ControlKind::slider;
    if (s == "button") return ControlKind::button;
    if (s == "label") return ControlKind::label;
    if (s == "combo") return ControlKind::combo;
    if (s == "meter") return ControlKind::meter;
    if (s == "keyboard") return ControlKind::keyboard;
    if (s == "presetBar") return ControlKind::presetBar;
    return ControlKind::custom;
}

void parseControl (UiLayout& layout, const juce::String& id, const juce::var& node)
{
    ControlSpec spec;
    spec.id = id;
    if (auto* obj = node.getDynamicObject())
    {
        spec.kind = kindFromString (obj->getProperty ("type").toString());
        if (obj->hasProperty ("x"))
        {
            const int x = static_cast<int> (obj->getProperty ("x"));
            const int y = static_cast<int> (obj->getProperty ("y"));
            const int w = static_cast<int> (obj->getProperty ("w"));
            const int h = static_cast<int> (obj->getProperty ("h"));
            spec.bounds = { x, y, w, h };
        }
        spec.legend = obj->getProperty ("legend").toString();
        spec.tooltip = obj->getProperty ("tooltip").toString();
        spec.filmstripAsset = obj->getProperty ("filmstrip").toString();
        spec.filmstripFrames = static_cast<int> (obj->getProperty ("filmstripFrames"));
    }
    layout.controls[id] = spec;
}

} // namespace

UiLayout UiLayout::loadFromJson (const juce::String& jsonText)
{
    UiLayout layout;
    const auto parsed = juce::JSON::parse (jsonText);
    if (auto* root = parsed.getDynamicObject())
    {
        if (root->hasProperty ("window"))
        {
            if (auto* win = root->getProperty ("window").getDynamicObject())
            {
                layout.windowWidth = static_cast<int> (win->getProperty ("width"));
                layout.windowHeight = static_cast<int> (win->getProperty ("height"));
                layout.resizable = static_cast<bool> (win->getProperty ("resizable"));
                layout.backgroundAsset = win->getProperty ("background").toString();
            }
        }

        if (auto* controls = root->getProperty ("controls").getDynamicObject())
        {
            for (const auto& id : controls->getProperties())
                parseControl (layout, id.name.toString(), id.value);
        }
    }
    return layout;
}

UiLayout UiLayout::loadFromBinary (const void* data, std::size_t size)
{
    if (data == nullptr || size == 0)
        return {};
    return loadFromJson (juce::String::fromUTF8 (static_cast<const char*> (data), static_cast<int> (size)));
}

const ControlSpec* UiLayout::find (const juce::String& id) const noexcept
{
    const auto it = controls.find (id);
    return it != controls.end() ? &it->second : nullptr;
}

} // namespace dl_ui