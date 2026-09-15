#pragma once

#include <JuceHeader.h>
#include "Model/AppMode.h"

namespace resonance::view
{
class ModeToolbar : public juce::Component
{
public:
    using ModeChanged = std::function<void(resonance::model::AppMode)>;

    ModeToolbar();

    void setCurrentMode(resonance::model::AppMode mode);
    void onModeChanged(ModeChanged callback) { modeChanged = std::move(callback); }

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void buildButtons();

    juce::OwnedArray<juce::TextButton> modeButtons;
    resonance::model::AppMode current = resonance::model::AppMode::Electribe;
    ModeChanged modeChanged;
};
} // namespace resonance::view
