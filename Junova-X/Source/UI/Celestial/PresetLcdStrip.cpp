#include "PresetLcdStrip.h"

namespace junovax::ui::celestial
{
PresetLcdStrip::PresetLcdStrip()
{
    for (auto* b : { &prev_, &next_ })
        addAndMakeVisible (*b);
}

void PresetLcdStrip::setPresetText (const juce::String& text)
{
    presetText_ = text;
    repaint();
}

juce::String PresetLcdStrip::getPresetText() const
{
    return presetText_;
}

void PresetLcdStrip::paint (juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat();
    g.setColour (Theme::panelGrey().brighter (0.08f));
    g.fillRoundedRectangle (r.reduced (0.5f), 6.0f);
    g.setColour (Theme::electricCyan().withAlpha (0.55f));
    g.drawRoundedRectangle (r.reduced (0.5f), 6.0f, 1.2f);

    auto lcd = r.reduced (32.0f, 4.0f);
    g.setColour (Theme::background().darker (0.2f));
    g.fillRoundedRectangle (lcd, 4.0f);
    g.setFont (Theme::readoutFont (12.0f));
    g.setColour (Theme::electricCyan());
    g.drawText (presetText_, lcd, juce::Justification::centred);
}

void PresetLcdStrip::resized()
{
    auto area = getLocalBounds();
    prev_.setBounds (area.removeFromLeft (28).reduced (2));
    next_.setBounds (area.removeFromRight (28).reduced (2));
}
} // namespace junovax::ui::celestial