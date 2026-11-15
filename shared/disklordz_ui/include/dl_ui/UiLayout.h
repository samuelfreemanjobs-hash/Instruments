#pragma once

#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>

#include <unordered_map>

namespace dl_ui
{

enum class ControlKind
{
    background,
    knob,
    slider,
    button,
    label,
    combo,
    meter,
    keyboard,
    presetBar,
    custom
};

struct ControlSpec final
{
    juce::String id;
    ControlKind kind = ControlKind::custom;
    juce::Rectangle<int> bounds;
    juce::String legend;
    juce::String tooltip;
    juce::String filmstripAsset;
    int filmstripFrames = 0;
};

struct UiLayout final
{
    int windowWidth = 960;
    int windowHeight = 640;
    bool resizable = true;
    juce::String backgroundAsset;
    std::unordered_map<juce::String, ControlSpec> controls;

    static UiLayout loadFromJson (const juce::String& jsonText);
    static UiLayout loadFromBinary (const void* data, std::size_t size);
    const ControlSpec* find (const juce::String& id) const noexcept;
};

} // namespace dl_ui