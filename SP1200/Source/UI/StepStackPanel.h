#pragma once

#include "../Engine/PatternSequencer.h"

#include <juce_gui_basics/juce_gui_basics.h>

class StepStackPanel : public juce::Component
{
public:
    enum class Mode
    {
        velocity,
        pitch,
        pan,
        filter
    };

    explicit StepStackPanel (sp1200::PatternSequencer& sequencer);

    void setSelectedPad (int padIndex);
    [[nodiscard]] int selectedPad() const noexcept { return selectedPad_; }

    void setMode (Mode mode);
    [[nodiscard]] Mode mode() const noexcept { return mode_; }

    void refresh();

private:
    void paint (juce::Graphics& g) override;
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;

    float valueForStep (int stepIndex) const;
    void setValueAtStep (int stepIndex, float normalized);

    sp1200::PatternSequencer& sequencer_;
    int selectedPad_ = 0;
    Mode mode_ = Mode::velocity;
};
