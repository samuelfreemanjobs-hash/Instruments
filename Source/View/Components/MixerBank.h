#pragma once

#include <JuceHeader.h>
#include "Controller/AppController.h"
#include "Audio/MixConsole.h"
#include "MixerChannelStrip.h"
#include <array>
#include <memory>

namespace vmpc::view
{
/** Eight channel strips with per-channel insert editors. */
class MixerBank : public juce::Component,
                  public juce::Button::Listener
{
public:
    explicit MixerBank(controller::AppController& controller);
    ~MixerBank() override;

    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void meterUpdate(int channel, float peak);

private:
    void openInsertsForChannel(int channelIndex);

    controller::AppController& appController;
    juce::Label title { {}, "Mixer channels" };
    std::array<std::unique_ptr<MixerChannelStrip>, vmpc::audio::MixConsole::kNumChannels> strips;
    std::array<juce::TextButton, vmpc::audio::MixConsole::kNumChannels> insertButtons;
    std::unique_ptr<juce::DocumentWindow> insertWindow;
};
} // namespace vmpc::view
