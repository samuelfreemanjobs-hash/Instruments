#pragma once

#include <JuceHeader.h>
#include "MoogStackLineage.h"

/**
 * V Voyager panel design tokens (UI_DESIGN_SYSTEM.md).
 * Hero reference: docs/UI_CONCEPT_VOYAGER-V.png
 */
namespace voyagerui
{
namespace colours
{
inline constexpr std::uint32_t kPanelMetalARGB = 0xff3a3d42;
inline constexpr std::uint32_t kPanelMetalHiARGB = 0xff5c6168;
inline constexpr std::uint32_t kPanelInsetARGB = 0xff25282c;
inline constexpr std::uint32_t kPanelWoodARGB = 0xff6b4423;
inline constexpr std::uint32_t kPanelWoodDarkARGB = 0xff3d2614;
inline constexpr std::uint32_t kDefaultAccentARGB = 0xfff4a62a;
inline constexpr std::uint32_t kSectionTextARGB = 0xfff0f2f5;
inline constexpr std::uint32_t kTitleGoldARGB = 0xffe8b44a;

inline const juce::Colour panelMetal { kPanelMetalARGB };
inline const juce::Colour panelMetalHi { kPanelMetalHiARGB };
inline const juce::Colour panelInset { kPanelInsetARGB };
inline const juce::Colour panelWood { kPanelWoodARGB };
inline const juce::Colour panelWoodDark { kPanelWoodDarkARGB };
inline const juce::Colour defaultAccent { kDefaultAccentARGB };
inline const juce::Colour sectionText { kSectionTextARGB };
inline const juce::Colour titleGold { kTitleGoldARGB };

inline juce::Colour accentForLineage(moogstack::Lineage lineage) noexcept
{
    switch (lineage)
    {
        case moogstack::Lineage::voyager:
            return juce::Colour(0xfff4a62a);
        case moogstack::Lineage::subPhatty:
            return juce::Colour(0xffe85d4a);
        case moogstack::Lineage::subsequent37:
            return juce::Colour(0xff4ec9e0);
        case moogstack::Lineage::modelD:
            return juce::Colour(0xff6fa8dc);
    }
    return defaultAccent;
}
} // namespace colours

namespace type
{
inline constexpr float titlePt = 30.0f;
inline constexpr float subtitlePt = 11.0f;
inline constexpr float sectionTitlePt = 13.0f;
inline constexpr float sectionFramePt = 14.0f;
inline constexpr float controlLabelPt = 11.0f;
inline constexpr float microLabelPt = 9.0f;
inline constexpr float captionPt = 8.5f;
} // namespace type

namespace layout
{
inline constexpr int defaultEditorWidthPx = 1180;
inline constexpr int defaultEditorHeightPx = 820;
inline constexpr int minEditorWidthPx = 1000;
inline constexpr int minEditorHeightPx = 680;
inline constexpr int maxEditorWidthPx = 1520;
inline constexpr int maxEditorHeightPx = 980;

inline constexpr int woodRailWidthPx = 44;
inline constexpr int headerBandHeightPx = 122;
inline constexpr int headerInsetHeightPx = 112;
inline constexpr int panelMarginPx = 12;
inline constexpr int performanceStripHeightPx = 94;

inline constexpr int oscSectionHeightPx = 195;
inline constexpr int filterSectionHeightPx = 148;
inline constexpr int envSectionHeightPx = 158;
inline constexpr int modSectionHeightPx = 128;

inline constexpr int filterCutoffMaxWidthPx = 148;
inline constexpr int sectionCornerRadiusPx = 8;
inline constexpr int ledDiameterPx = 6;
inline constexpr int ledVerticalSpacingPx = 26;
} // namespace layout

namespace components
{
inline constexpr int stackModeRadioGroupId = 88001;
inline constexpr int oversampleRadioGroupId = 88002;
inline constexpr int filterKbTrackRadioGroupId = 88004;
inline constexpr int waveSelectorRadioBaseId = 90110;
} // namespace components
} // namespace voyagerui
