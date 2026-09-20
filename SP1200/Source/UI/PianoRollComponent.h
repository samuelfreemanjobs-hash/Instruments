#pragma once

#include "../Engine/PatternSequencer.h"

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

class PianoRollComponent : public juce::Component
{
public:
    PianoRollComponent (sp1200::PatternSequencer& sequencer,
                        std::function<juce::String (int padIndex)> padLabelFn);

    void setChromaticMode (bool on) { chromaticMode_ = on; repaint(); }
    [[nodiscard]] bool chromaticMode() const noexcept { return chromaticMode_; }

    void setChromaticTune (float semitones) { chromaticTune_ = semitones; }
    [[nodiscard]] float chromaticTune() const noexcept { return chromaticTune_; }

    void refreshFromPattern();

private:
    void paint (juce::Graphics& g) override;
    void mouseDown (const juce::MouseEvent& e) override;

    [[nodiscard]] juce::Rectangle<int> gridArea() const;
    bool cellFromPoint (juce::Point<int> pt, int& padOut, int& stepOut) const;

    sp1200::PatternSequencer& sequencer_;
    std::function<juce::String (int padIndex)> padLabelFn_;
    bool chromaticMode_ = false;
    float chromaticTune_ = 0.0f;

    static juce::Colour laneColour (int pad);
};
