#include "StepSequencerGrid.h"

namespace vmpc::view
{
StepSequencerGrid::StepSequencerGrid()
{
    setOpaque(false);
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

void StepSequencerGrid::paint(juce::Graphics& g)
{
    const int cols = 4;
    const int rows = 4;
    const int pad = 6;
    const int w = (getWidth() - pad * (cols + 1)) / cols;
    const int h = (getHeight() - pad * (rows + 1)) / rows;

    for (int i = 0; i < vmpc::model::SixteenStepPattern::kNumSteps; ++i)
    {
        const int col = i % cols;
        const int row = i / cols;
        auto r = juce::Rectangle<int>(pad + col * (w + pad), pad + row * (h + pad), w, h);
        drawPad(g, r, patternCopy.getStep(i).active, i == playingStep);
    }
}

void StepSequencerGrid::drawPad(juce::Graphics& g, juce::Rectangle<int> r, bool active, bool playing) const
{
    const auto face = juce::Colour(0xff2a2a2e);
    const auto ledOn = juce::Colour(0xff3fffd6);
    g.setColour(face);
    g.fillRoundedRectangle(r.toFloat(), 4.0f);
    if (active)
    {
        g.setColour(ledOn.withAlpha(0.75f));
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
    const int cols = 4;
    const int rows = 4;
    const int pad = 6;
    const int w = (getWidth() - pad * (cols + 1)) / cols;
    const int h = (getHeight() - pad * (rows + 1)) / rows;

    for (int i = 0; i < vmpc::model::SixteenStepPattern::kNumSteps; ++i)
    {
        const int col = i % cols;
        const int row = i / cols;
        auto r = juce::Rectangle<int>(pad + col * (w + pad), pad + row * (h + pad), w, h);
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
