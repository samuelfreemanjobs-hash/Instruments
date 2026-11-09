#include "ValueReadout.h"

namespace fds {
namespace gui {

ValueReadout::ValueReadout()
{
    valueLabel.setJustificationType (juce::Justification::centredLeft);
    valueLabel.setColour (juce::Label::textColourId, argb (color::text::primary));
    valueLabel.setFont (makeFont (typography::style::valueLarge));
    valueLabel.setEditable (true, false, true);
    valueLabel.onTextChange = [this]
    {
        if (onTextCommit)
            onTextCommit (valueLabel.getText());
    };
    addAndMakeVisible (valueLabel);

    unitLabel.setJustificationType (juce::Justification::centredRight);
    unitLabel.setColour (juce::Label::textColourId, argb (color::text::tertiary));
    unitLabel.setFont (makeFont (typography::style::micro));
    unitLabel.setInterceptsMouseClicks (false, false);
    addAndMakeVisible (unitLabel);

    setSize (96, 28);
}

void ValueReadout::setLarge (bool isLarge)
{
    large = isLarge;
    valueLabel.setFont (makeFont (large ? typography::style::valueLarge : typography::style::value));
}

void ValueReadout::setState (State s)
{
    state = s;
    const auto col = (state == State::over) ? argb (color::state::danger) : argb (color::text::primary);
    valueLabel.setColour (juce::Label::textColourId, col);
    unitLabel.setColour (juce::Label::textColourId,
                         state == State::over ? argb (color::state::danger) : argb (color::text::tertiary));
    repaint();
}

void ValueReadout::setValueText (const juce::String& value, const juce::String& unit)
{
    if (! valueLabel.isBeingEdited())
        valueLabel.setText (value, juce::dontSendNotification);
    unitLabel.setText (state == State::over ? "OVER" : unit, juce::dontSendNotification);
}

void ValueReadout::setFromDb (float db)
{
    if (db <= meter::numericFloorDb || ! std::isfinite (db))
    {
        setState (State::floor);
        setValueText (minusSign() + infinityGlyph(), "dB");
        return;
    }
    if (db > meter::overThresholdDb)
    {
        setState (State::over);
        setValueText (formatDb (db), "OVER");
        return;
    }
    setState (State::normal);
    setValueText (formatDb (db), "dB");
}

void ValueReadout::paint (juce::Graphics& g)
{
    g.setColour (argb (color::bg::well));
    g.fillPath (wellPath);
    // well elevation: inner shadow approximation
    g.setColour (argb (elevation::wellInner.color).withAlpha (elevation::wellInner.alpha * 0.5f));
    g.strokePath (wellPath, juce::PathStrokeType (1.0f));
}

void ValueReadout::resized()
{
    wellPath.clear();
    wellPath.addRoundedRectangle (getLocalBounds().toFloat(), radius::xs);
    auto r = getLocalBounds().reduced (8, 4);
    unitLabel.setBounds (r.removeFromRight (40));
    valueLabel.setBounds (r);
}

void ValueReadout::mouseDown (const juce::MouseEvent&)
{
    valueLabel.showEditor();
}

} // namespace gui
} // namespace fds
