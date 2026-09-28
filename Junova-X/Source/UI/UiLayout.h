#pragma once

// Layout constants — replace with values from your GUI design export (Figma → UiLayout).
// Hermes GUI seat updates this file; MainPanel reads bounds from here.

namespace junovax::ui
{
struct Layout
{
    static constexpr int editorWidth = 920;
    static constexpr int editorHeight = 560;

    static constexpr int headerHeight = 72;
    static constexpr int tabBarHeight = 36;

    static constexpr int margin = 16;
    static constexpr int knobSize = 72;
    static constexpr int knobGap = 12;
};
} // namespace junovax::ui
