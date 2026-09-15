#include "StepSequencerGrid.h"

namespace vmpc::view
{
StepSequencerGrid::StepSequencerGrid()
{
    setOpaque(false);
}

void StepSequencerGrid::setLayout(Layout newLayout)
{
    layout = newLayout;
    repaint();
}

bool StepSequencerGrid::isStepActive(int index) const
{
    if (index < 0 || index >= vmpc::model::SixteenStepPattern::kNumSteps)
        return false;
    return patternCopy.getStep(index).active;
}

void StepSequencerGrid::setPattern(const vmpc::model::SixteenStepPattern& pattern)
{
    for (int i = 0; i < vmpc::model::SixteenStepPattern::kNumSteps; ++i)
        patternCopy.getStep(i) = pattern.getStep(i);
    repaint();
}

void StepSequencerGrid::setPlayingStep(int step)
{
    playingStep = step;
    repaint();
}

void StepSequencerGrid::layoutGrid4x4(juce::Rectangle<int> area, int stepIndex, juce::Rectangle<int>& out) const
{
    const int cols = 4;
    const int pad = 6;
    const int w = (area.getWidth() - pad * (cols + 1)) / cols;
    const int h = (area.getHeight() - pad * (cols + 1)) / cols;
    const int col = stepIndex % cols;
    const int row = stepIndex / cols;
    out = juce::Rectangle<int>(area.getX() + pad + col * (w + pad), area.getY() + pad + row * (h + pad), w, h);
}

void StepSequencerGrid::layoutRow16(juce::Rectangle<int> area, int stepIndex, juce::Rectangle<int>& out) const
{
    const int pad = 4;
    const int w = (area.getWidth() - pad * 17) / 16;
    const int h = area.getHeight() - pad * 2;
    out = juce::Rectangle<int>(area.getX() + pad + stepIndex * (w + pad), area.getY() + pad, w, h);
}

void StepSequencerGrid::paint(juce::Graphics& g)
{
    const auto area = getLocalBounds();

    for (int i = 0; i < vmpc::model::SixteenStepPattern::kNumSteps; ++i)
    {
        juce::Rectangle<int> r;
        if (layout == Layout::Row16Electribe)
            layoutRow16(area, i, r);
        else
            layoutGrid4x4(area, i, r);

        drawPad(g, r, patternCopy.getStep(i).active, i == playingStep);

        if (layout == Layout::Row16Electribe)
        {
            g.setColour(juce::Colours::white.withAlpha(0.35f));
            g.setFont(10.0f);
            g.drawText(juce::String(i + 1), r, juce::Justification::centredBottom);
        }
    }
}

void StepSequencerGrid::drawPad(juce::Graphics& g, juce::Rectangle<int> r, bool active, bool playing) const
{
    const auto face = juce::Colour(0xff2a2a2e);
    const auto ledOn = juce::Colour(0xff3fffd6);
    g.setColour(face);
    g.fillRoundedRectangle(r.toFloat(), layout == Layout::Row16Electribe ? 3.0f : 4.0f);
    if (active)
    {
        g.setColour(ledOn.withAlpha(0.85f));
        if (layout == Layout::Row16Electribe)
            g.fillRoundedRectangle(r.reduced(3).toFloat(), 2.0f);
        else
            g.fillEllipse(r.getCentreX() - 5.0f, r.getY() + 4.0f, 10.0f, 10.0f);
    }
    if (playing)
    {
        g.setColour(ledOn);
        g.drawRoundedRectangle(r.toFloat().reduced(1.0f), 4.0f, 2.0f);
    }
}

int StepSequencerGrid::hitTestStep(juce::Point<int> pos) const
{
    const auto area = getLocalBounds();

    for (int i = 0; i < vmpc::model::SixteenStepPattern::kNumSteps; ++i)
    {
        juce::Rectangle<int> r;
        if (layout == Layout::Row16Electribe)
            layoutRow16(area, i, r);
        else
            layoutGrid4x4(area, i, r);

        if (r.contains(pos))
            return i;
    }
    return -1;
}

void StepSequencerGrid::mouseDown(const juce::MouseEvent& e)
{
    const int step = hitTestStep(e.getPosition());
    if (step < 0)
        return;

    auto& s = patternCopy.getStep(step);
    s.active = !s.active;
    repaint();
    if (stepToggled)
        stepToggled(step, s.active);
}
} // namespace vmpc::view
