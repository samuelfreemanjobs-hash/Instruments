#include "HeaderBar.h"

namespace fds {
namespace gui {

HeaderBar::HeaderBar (const juce::String& productNameText, const juce::String& versionText)
{
    addAndMakeVisible (glyphSlot);

    productName.setText (applyCase (productNameText, typography::style::productName.textCase),
                         juce::dontSendNotification);
    productName.setFont (makeFont (typography::style::productName));
    productName.setColour (juce::Label::textColourId, argb (color::text::primary));
    productName.setInterceptsMouseClicks (false, false);
    addAndMakeVisible (productName);

    brandLabel.setText (brandName, juce::dontSendNotification);
    brandLabel.setFont (makeFont (typography::style::micro));
    brandLabel.setColour (juce::Label::textColourId, argb (color::text::tertiary));
    brandLabel.setInterceptsMouseClicks (false, false);
    addAndMakeVisible (brandLabel);

    addAndMakeVisible (presets);

    versionLabel.setText (versionText, juce::dontSendNotification);
    versionLabel.setFont (makeFont (typography::style::micro));
    versionLabel.setColour (juce::Label::textColourId, argb (color::text::tertiary));
    versionLabel.setJustificationType (juce::Justification::centredRight);
    versionLabel.setInterceptsMouseClicks (false, false);
    addAndMakeVisible (versionLabel);

    addAndMakeVisible (bypass);
    setSize (640, size::headerHeight);
}

void HeaderBar::setProductName (const juce::String& name)
{
    productName.setText (applyCase (name, typography::style::productName.textCase),
                         juce::dontSendNotification);
}

void HeaderBar::setVersion (const juce::String& version)
{
    versionLabel.setText (version, juce::dontSendNotification);
}

void HeaderBar::setGlyphPath (juce::Path glyph)
{
    glyphSlot.path = std::move (glyph);
    glyphSlot.repaint();
}

void HeaderBar::paint (juce::Graphics& g)
{
    g.fillAll (argb (color::bg::chassis));
    g.setColour (argb (color::border::divider));
    g.fillRect (0, getHeight() - 1, getWidth(), 1);
}

void HeaderBar::resized()
{
    auto r = getLocalBounds().reduced (size::windowPadding / 2, 0);
    glyphSlot.setBounds (r.removeFromLeft (22).withSizeKeepingCentre (22, 22));
    r.removeFromLeft (8);
    auto left = r.removeFromLeft (180);
    productName.setBounds (left.removeFromTop (22));
    brandLabel.setBounds (left);

    auto right = r.removeFromRight (140);
    bypass.setBounds (right.removeFromRight (88).withSizeKeepingCentre (88, size::toggle::md));
    versionLabel.setBounds (right);

    presets.setBounds (r.withSizeKeepingCentre (juce::jmin (240, r.getWidth()), size::toggle::md));
}

} // namespace gui
} // namespace fds
