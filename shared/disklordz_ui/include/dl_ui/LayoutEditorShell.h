#pragma once

#include "dl_ui/SkinAssetCache.h"
#include "dl_ui/UiLayout.h"

#include <juce_gui_basics/juce_gui_basics.h>

namespace dl_ui
{

class LayoutEditorShell final
{
public:
    static void applyLayout (juce::Component& root, const UiLayout& layout);
    static void paintBackground (juce::Graphics& g, const juce::Component& editor, const UiLayout& layout,
                                 SkinAssetCache& assets);
};

} // namespace dl_ui