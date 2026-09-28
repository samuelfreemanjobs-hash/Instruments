#include "StepStackPanel.h"

#include "../SP1200Constants.h"

namespace
{
sp1200::PatternStep* findStep (sp1200::PatternSequencer& seq, int pad, int stepIndex)
{
    auto& p = seq.pattern (seq.currentPattern());
    for (auto& st : p.steps)
        if (st.pad == pad && st.stepIndex == stepIndex)
            return &st;
    return nullptr;
}
} // namespace

StepStackPanel::StepStackPanel (sp1200::PatternSequencer& sequencer)
    : sequencer_ (sequencer)
{
    setOpaque (true);
}

void StepStackPanel::setSelectedPad (int padIndex)
{
    selectedPad_ = std::clamp (padIndex, 0, sp1200::kNumPads - 1);
    repaint();
}

void StepStackPanel::setMode (Mode mode)
{
    mode_ = mode;
    repaint();
}

void StepStackPanel::refresh()
{
    repaint();
}

float StepStackPanel::valueForStep (int stepIndex) const
{
    const auto* st = findStep (const_cast<sp1200::PatternSequencer&> (sequencer_), selectedPad_, stepIndex);
    if (st == nullptr)
        return 0.0f;

    switch (mode_)
    {
        case Mode::velocity:
            return st->velocity;
        case Mode::pitch:
            return (st->tuneSemitones + 12.0f) / 24.0f;
        case Mode::pan:
            return (st->pan + 1.0f) * 0.5f;
        case Mode::filter:
            return st->filterCutoff;
    }
    return 0.0f;
}

void StepStackPanel::setValueAtStep (int stepIndex, float normalized)
{
    auto* st = findStep (sequencer_, selectedPad_, stepIndex);
    if (st == nullptr)
        return;

    normalized = std::clamp (normalized, 0.0f, 1.0f);
    switch (mode_)
    {
        case Mode::velocity:
            st->velocity = normalized;
            break;
        case Mode::pitch:
            st->tuneSemitones = sp1200::quantizeToMultiPitch (normalized * 24.0f - 12.0f);
            break;
        case Mode::pan:
            st->pan = normalized * 2.0f - 1.0f;
            break;
        case Mode::filter:
            st->filterCutoff = normalized;
            break;
    }
    repaint();
}

void StepStackPanel::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff121218));

    const char* labels[] = { "VELOCITY (HIT)", "PITCH TUNE", "PAN (L/R)", "SSM FILTER CUTOFF" };
    g.setColour (juce::Colours::white.withAlpha (0.9f));
    g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
    g.drawText (labels[static_cast<int> (mode_)], getLocalBounds().removeFromTop (18).reduced (4, 0),
                juce::Justification::centredLeft);

    const auto area = getLocalBounds().reduced (4).withTrimmedTop (18);
    const int totalSteps = std::max (1, sequencer_.pattern (sequencer_.currentPattern()).totalSteps());
    const int barW = std::max (4, area.getWidth() / totalSteps);

    for (int step = 0; step < totalSteps; ++step)
    {
        const float v = valueForStep (step);
        if (v <= 0.001f && ! findStep (sequencer_, selectedPad_, step))
            continue;

        const int x = area.getX() + step * barW;
        const int h = static_cast<int> (v * static_cast<float> (area.getHeight()));
        g.setColour (juce::Colour (0xff3d8bfd));
        g.fillRect (x + 1, area.getBottom() - h, barW - 2, h);
    }
}

void StepStackPanel::mouseDown (const juce::MouseEvent& e)
{
    mouseDrag (e);
}

void StepStackPanel::mouseDrag (const juce::MouseEvent& e)
{
    const auto area = getLocalBounds().reduced (4).withTrimmedTop (18);
    if (! area.contains (e.getPosition()))
        return;

    const int totalSteps = std::max (1, sequencer_.pattern (sequencer_.currentPattern()).totalSteps());
    const int barW = std::max (4, area.getWidth() / totalSteps);
    const int step = std::clamp ((e.x - area.getX()) / barW, 0, totalSteps - 1);
    const float norm = 1.0f - static_cast<float> (e.y - area.getY()) / static_cast<float> (area.getHeight());
    setValueAtStep (step, norm);
}
