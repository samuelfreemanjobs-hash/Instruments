#pragma once

#include <JuceHeader.h>
#include "Model/Sequencer/SixteenStepPattern.h"

namespace vmpc::view
{
class StepSequencerGrid : public juce::Component
{
public:
    enum class Layout
    {
        Grid4x4,
        Row16Electribe
    };

    using StepToggled = std::function<void(int stepIndex, bool active)>;

    StepSequencerGrid();

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;

    void setLayout(Layout newLayout);
    void setPattern(const vmpc::model::SixteenStepPattern& pattern);
    void setPlayingStep(int step);

    void onStepToggled(StepToggled callback) { stepToggled = std::move(callback); }

    bool isStepActive(int index) const;

private:
    int hitTestStep(juce::Point<int> pos) const;
    void drawPad(juce::Graphics& g, juce::Rectangle<int> r, bool active, bool playing) const;
    void layoutGrid4x4(juce::Rectangle<int> area, int stepIndex, juce::Rectangle<int>& out) const;
    void layoutRow16(juce::Rectangle<int> area, int stepIndex, juce::Rectangle<int>& out) const;

    Layout layout = Layout::Grid4x4;
    vmpc::model::SixteenStepPattern patternCopy;
    int playingStep = -1;
    StepToggled stepToggled;
};
} // namespace vmpc::view
