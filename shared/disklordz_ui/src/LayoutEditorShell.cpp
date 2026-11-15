#include "dl_ui/LayoutEditorShell.h"

namespace dl_ui
{

namespace
{

void placeById (juce::Component& root, const juce::String& id, juce::Rectangle<int> bounds)
{
    if (auto* child = root.findChildWithID (id))
        child->setBounds (bounds);
}

} // namespace

void LayoutEditorShell::applyLayout (juce::Component& root, const UiLayout& layout)
{
    for (const auto& [id, spec] : layout.controls)
    {
        if (spec.kind == ControlKind::background)
            continue;
        placeById (root, id, spec.bounds);
    }
}

void LayoutEditorShell::paintBackground (juce::Graphics& g, const juce::Component&, const UiLayout& layout,
                                         SkinAssetCache& assets)
{
    if (! layout.backgroundAsset.isEmpty())
    {
        if (auto bg = assets.getImage (layout.backgroundAsset); bg.isValid())
        {
            g.drawImage (bg, 0, 0, layout.windowWidth, layout.windowHeight, 0, 0, bg.getWidth(), bg.getHeight());
            return;
        }
    }
    g.fillAll (juce::Colour (0xff12141a));
}

} // namespace dl_ui