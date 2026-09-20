#include "PianoRollComponent.h"

#include "../SP1200Constants.h"

namespace
{
constexpr int kLabelWidth = 72;
constexpr int kHeaderHeight = 22;
} // namespace

PianoRollComponent::PianoRollComponent (sp1200::PatternSequencer& sequencer,
                                        std::function<juce::String (int padIndex)> padLabelFn)
    : sequencer_ (sequencer), padLabelFn_ (std::move (padLabelFn))
{
    setOpaque (true);
}

void PianoRollComponent::refreshFromPattern()
{
    repaint();
}

juce::Colour PianoRollComponent::laneColour (int pad)
{
    const float hue = static_cast<float> (pad) / static_cast<float> (sp1200::kNumPads);
    return juce::Colour::fromHSV (hue, 0.55f, 0.85f, 1.0f);
}

juce::Rectangle<int> PianoRollComponent::gridArea() const
{
    return getLocalBounds().withTrimmedLeft (kLabelWidth).withTrimmedTop (kHeaderHeight);
}

bool PianoRollComponent::cellFromPoint (juce::Point<int> pt, int& padOut, int& stepOut) const
{
    const auto grid = gridArea();
    if (! grid.contains (pt))
        return false;

    const auto& pat = sequencer_.pattern (sequencer_.currentPattern());
    const int totalSteps = std::max (1, pat.totalSteps());
    const int laneH = std::max (8, grid.getHeight() / sp1200::kNumPads);
    const int stepW = std::max (6, grid.getWidth() / totalSteps);

    const int localY = pt.y - grid.getY();
    const int localX = pt.x - grid.getX();
    padOut = std::clamp (localY / laneH, 0, sp1200::kNumPads - 1);
    stepOut = std::clamp (localX / stepW, 0, totalSteps - 1);
    return true;
}

void PianoRollComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1a1a1e));

    const auto& pat = sequencer_.pattern (sequencer_.currentPattern());
    const int totalSteps = std::max (1, pat.totalSteps());
    const auto grid = gridArea();
    const int laneH = std::max (8, grid.getHeight() / sp1200::kNumPads);
    const int stepW = std::max (6, grid.getWidth() / totalSteps);

    g.setColour (juce::Colours::white.withAlpha (0.85f));
    g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
    g.drawText ("MOD 20 PIANO ROLL | 1/16 @ 96 PPQN", 4, 2, getWidth() - 8, kHeaderHeight,
                juce::Justification::centredLeft);

    for (int pad = 0; pad < sp1200::kNumPads; ++pad)
    {
        const int y = grid.getY() + pad * laneH;
        g.setColour (juce::Colour (0xff2a2a30));
        g.fillRect (0, y, kLabelWidth, laneH);
        g.setColour (juce::Colours::white.withAlpha (0.9f));
        juce::String label = "P" + juce::String (pad + 1).paddedLeft ('0', 2);
        if (padLabelFn_)
            label = padLabelFn_ (pad);
        g.drawText (label, 4, y, kLabelWidth - 6, laneH, juce::Justification::centredLeft);

        g.setColour (juce::Colour (0xff252528));
        g.fillRect (grid.getX(), y, grid.getWidth(), laneH);
    }

    for (int step = 0; step <= totalSteps; ++step)
    {
        const int x = grid.getX() + step * stepW;
        if (step % sp1200::kStepsPerBar == 0)
            g.setColour (juce::Colours::white.withAlpha (0.35f));
        else if (step % 4 == 0)
            g.setColour (juce::Colours::white.withAlpha (0.12f));
        else
            g.setColour (juce::Colours::white.withAlpha (0.05f));
        g.drawVerticalLine (x, static_cast<float> (grid.getY()),
                            static_cast<float> (grid.getBottom()));
    }

    for (const auto& st : pat.steps)
    {
        if (st.stepIndex < 0 || st.stepIndex >= totalSteps || st.pad < 0 || st.pad >= sp1200::kNumPads)
            continue;

        const int x = grid.getX() + st.stepIndex * stepW + 1;
        const int y = grid.getY() + st.pad * laneH + 2;
        const int w = std::max (4, stepW - 2);
        const int h = std::max (4, laneH - 4);

        g.setColour (laneColour (st.pad));
        g.fillRoundedRectangle (static_cast<float> (x), static_cast<float> (y),
                                static_cast<float> (w), static_cast<float> (h), 2.0f);

        if (chromaticMode_ && std::abs (st.tuneSemitones) > 0.01f)
        {
            g.setColour (juce::Colours::white);
            g.setFont (juce::FontOptions (9.0f));
            g.drawText (juce::String (st.tuneSemitones, 1), x, y, w, h, juce::Justification::centred);
        }
    }

    if (sequencer_.isPlaying())
    {
        const int playStep = sequencer_.playbackStepIndex();
        const int x = grid.getX() + playStep * stepW + stepW / 2;
        g.setColour (juce::Colours::yellow.withAlpha (0.9f));
        g.drawLine (static_cast<float> (x), static_cast<float> (grid.getY()),
                    static_cast<float> (x), static_cast<float> (grid.getBottom()), 2.0f);
    }
}

void PianoRollComponent::mouseDown (const juce::MouseEvent& e)
{
    int pad = 0, step = 0;
    if (! cellFromPoint (e.getPosition(), pad, step))
        return;

    sequencer_.toggleStep (pad, step, 0.9f, chromaticTune_, chromaticMode_);
    repaint();
}
