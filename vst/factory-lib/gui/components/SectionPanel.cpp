#include "SectionPanel.h"

namespace fds {
namespace gui {

SectionPanel::SectionPanel (const juce::String& titleText)
{
    title.setText (applyCase (titleText, typography::style::sectionTitle.textCase),
                   juce::dontSendNotification);
    title.setColour (juce::Label::textColourId, argb (color::text::secondary));
    title.setFont (makeFont (typography::style::sectionTitle));
    title.setInterceptsMouseClicks (false, false);
    addAndMakeVisible (title);
    setSize (200, 120);
}

void SectionPanel::setTitle (const juce::String& titleText)
{
    title.setText (applyCase (titleText, typography::style::sectionTitle.textCase),
                   juce::dontSendNotification);
}

juce::Rectangle<int> SectionPanel::getContentArea() const
{
    return getLocalBounds().reduced (12).withTrimmedTop (title.getText().isEmpty() ? 0 : 18);
}

void SectionPanel::paint (juce::Graphics& g)
{
    g.setColour (argb (color::bg::section));
    g.fillPath (bodyPath);
}

void SectionPanel::resized()
{
    bodyPath.clear();
    bodyPath.addRoundedRectangle (getLocalBounds().toFloat(), radius::md);
    title.setBounds (12, 8, getWidth() - 24, 14);
}

} // namespace gui
} // namespace fds
