#include "PianoRollGrid.h"

namespace resonance::view
{
PianoRollGrid::PianoRollGrid()
{
    setOpaque(true);
}

void PianoRollGrid::paint(juce::Graphics& g)
{
    const auto bg = juce::Colour(0xff101018);
    g.fillAll(bg);

    const int keyWidth = 48;
    const auto gridArea = getLocalBounds().withTrimmedLeft(keyWidth);
    const float rowH = static_cast<float>(getHeight()) / static_cast<float>(kVisibleKeys);

    for (int k = 0; k < kVisibleKeys; ++k)
    {
        const int midi = 60 + (kVisibleKeys - 1 - k);
        const bool black = juce::MidiMessage::isMidiNoteBlack(midi);
        g.setColour(black ? juce::Colour(0xff1a1a22) : juce::Colour(0xff2c2c36));
        g.fillRect(0, static_cast<int>(k * rowH), keyWidth, static_cast<int>(rowH) + 1);

        g.setColour(juce::Colours::white.withAlpha(0.08f));
        g.drawHorizontalLine(static_cast<int>((k + 1) * rowH), static_cast<float>(keyWidth), static_cast<float>(getWidth()));
    }

    g.setColour(juce::Colour(0xff243848));
    const int beats = 16;
    for (int b = 0; b <= beats; ++b)
    {
        const float x = keyWidth + gridArea.getWidth() * (static_cast<float>(b) / static_cast<float>(beats));
        g.drawVerticalLine(static_cast<int>(x), 0.0f, static_cast<float>(getHeight()));
    }
}
} // namespace resonance::view
