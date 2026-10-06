#include "KeypadComponent.h"

KeypadComponent::KeypadComponent()
{
    for (int d = 0; d < 10; ++d)
    {
        auto& b = digitButtons_[static_cast<std::size_t> (d)];
        b.setButtonText (juce::String (d));
        b.setColour (juce::TextButton::buttonColourId, juce::Colour (0xfff2efe8));
        b.setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xffd4cfc6));
        b.setColour (juce::TextButton::textColourOffId, juce::Colours::black);
        b.setColour (juce::TextButton::textColourOnId, juce::Colours::black);
        b.onClick = [this, d] { if (onDigit) onDigit (d); };
        addAndMakeVisible (b);
    }
    enterButton_.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff7cb87c));
    enterButton_.setColour (juce::TextButton::textColourOffId, juce::Colours::black);
    cancelButton_.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffc08080));
    cancelButton_.setColour (juce::TextButton::textColourOffId, juce::Colours::black);
    enterButton_.onClick = [this] { if (onEnter) onEnter(); };
    cancelButton_.onClick = [this] { if (onCancel) onCancel(); };
    addAndMakeVisible (enterButton_);
    addAndMakeVisible (cancelButton_);
}

void KeypadComponent::paint (juce::Graphics& g)
{
    g.setColour (juce::Colour (0xff5c5650));
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 4.0f);
    g.setColour (juce::Colours::black.withAlpha (0.25f));
    g.drawRoundedRectangle (getLocalBounds().toFloat().reduced (0.5f), 4.0f, 1.0f);
}

void KeypadComponent::resized()
{
    auto r = getLocalBounds().reduced (4);
    const int cols = 3;
    const int rows = 4;
    const int cellW = r.getWidth() / cols;
    const int cellH = r.getHeight() / rows;

    static const int layout[12] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, 0, -2 };
    for (int i = 0; i < 12; ++i)
    {
        const int row = i / cols;
        const int col = i % cols;
        juce::Rectangle<int> cell (col * cellW, row * cellH, cellW, cellH);
        cell = cell.reduced (2);
        const int code = layout[i];
        if (code >= 0)
            digitButtons_[static_cast<std::size_t> (code)].setBounds (cell);
        else if (code == -1)
            cancelButton_.setBounds (cell);
        else
            enterButton_.setBounds (cell);
    }
}
