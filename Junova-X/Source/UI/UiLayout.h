#pragma once

// Celestial GUI artboard (matches design-reference-celestial.png).
namespace junovax::ui
{
struct Layout
{
    static constexpr int editorWidth = 1280;
    static constexpr int editorHeight = 840;

    static constexpr int headerHeight = 64;
    static constexpr int tabBarHeight = 28;

    static constexpr int margin = 16;
    static constexpr int knobSize = 72;
    static constexpr int knobGap = 12;
};
} // namespace junovax::ui
