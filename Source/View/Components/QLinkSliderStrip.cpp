#include "QLinkSliderStrip.h"

namespace vmpc::view
{
QLinkSliderStrip::QLinkSliderStrip()
{
    const char* defaultNames[] { "FILTER", "LEVEL", "ATTACK", "DECAY" };
    for (int i = 0; i < kNumLinks; ++i)
    {
        auto& s = sliders[static_cast<size_t>(i)];
        s.setSliderStyle(juce::Slider::LinearVertical);
        s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        s.setRange(0.0, 1.0, 0.001);
        s.setValue(0.5);
        s.onValueChange = [this, i]() {
            if (valueChanged)
                valueChanged(i, static_cast<float>(sliders[static_cast<size_t>(i)].getValue()));
        };
        addAndMakeVisible(s);

        labels[static_cast<size_t>(i)].setText(defaultNames[i], juce::dontSendNotification);
        labels[static_cast<size_t>(i)].setJustificationType(juce::Justification::centred);
        labels[static_cast<size_t>(i)].setFont(juce::FontOptions(9.0f));
        labels[static_cast<size_t>(i)].setColour(juce::Label::textColourId, juce::Colour(0xff6ec8e8));
        addAndMakeVisible(labels[static_cast<size_t>(i)]);
    }
}

void QLinkSliderStrip::setLinkLabel(int index, const juce::String& label)
{
    if (index >= 0 && index < kNumLinks)
        labels[static_cast<size_t>(index)].setText(label, juce::dontSendNotification);
}

void QLinkSliderStrip::setLinkValue(int index, float value01)
{
    if (index >= 0 && index < kNumLinks)
        sliders[static_cast<size_t>(index)].setValue(juce::jlimit(0.0, 1.0, static_cast<double>(value01)),
                                                     juce::dontSendNotification);
}

float QLinkSliderStrip::getLinkValue(int index) const
{
    if (index < 0 || index >= kNumLinks)
        return 0.0f;
    return static_cast<float>(sliders[static_cast<size_t>(index)].getValue());
}

void QLinkSliderStrip::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff141418));
    g.setColour(juce::Colour(0xff3d2b1f));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f), 4.0f, 1.0f);
    g.setColour(juce::Colours::white.withAlpha(0.5f));
    g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    g.drawText("Q-LINK", getLocalBounds().removeFromTop(14), juce::Justification::centred);
}

void QLinkSliderStrip::resized()
{
    auto area = getLocalBounds().reduced(4).withTrimmedTop(14);
    const int w = area.getWidth() / kNumLinks;
    for (int i = 0; i < kNumLinks; ++i)
    {
        auto col = area.removeFromLeft(w).reduced(2);
        labels[static_cast<size_t>(i)].setBounds(col.removeFromBottom(14));
        sliders[static_cast<size_t>(i)].setBounds(col);
    }
}
} // namespace vmpc::view
