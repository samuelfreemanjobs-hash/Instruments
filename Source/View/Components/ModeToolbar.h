#pragma once

#include <JuceHeader.h>
#include "Model/AppMode.h"

namespace vmpc::view
{
class ModeToolbar : public juce::Component
{
public:
    using ModeChanged = std::function<void(vmpc::model::AppMode)>;

    ModeToolbar();

    void setCurrentMode(vmpc::model::AppMode mode);
    void onModeChanged(ModeChanged callback) { modeChanged = std::move(callback); }

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void buildButtons();

    juce::OwnedArray<juce::TextButton> modeButtons;
    vmpc::model::AppMode current = vmpc::model::AppMode::Electribe;
    ModeChanged modeChanged;
};
} // namespace vmpc::view
