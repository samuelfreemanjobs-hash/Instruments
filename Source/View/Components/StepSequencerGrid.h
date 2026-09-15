#pragma once

#include <JuceHeader.h>
#include "Model/Sequencer/SixteenStepPattern.h"

namespace vmpc::view
{
class StepSequencerGrid : public juce::Component
{
public:
    using StepToggled = std::function<void(int stepIndex, bool active)>;

    StepSequencerGrid();

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;

    void setPattern(const vmpc::model::SixteenStepPattern& pattern);
    void setPlayingStep(int step);

    void onStepToggled(StepToggled callback) { stepToggled = std::move(callback); }

private:
    int hitTestStep(juce::Point<int> pos) const;
    void drawPad(juce::Graphics& g, juce::Rectangle<int> r, bool active, bool playing) const;

    vmpc::model::SixteenStepPattern patternCopy;
    int playingStep = -1;
    StepToggled stepToggled;
};
} // namespace vmpc::view
