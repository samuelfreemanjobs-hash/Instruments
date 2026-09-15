#pragma once

#include <JuceHeader.h>
#include "Controller/AppController.h"

namespace vmpc::view
{
class EsxTransportHeader : public juce::Component,
                           private controller::AppController::Listener
{
public:
    explicit EsxTransportHeader(controller::AppController& controller);
    ~EsxTransportHeader() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void transportStateChanged(bool playing) override;
    void appModeChanged(vmpc::model::AppMode) override {}
    void refreshLabels();

    controller::AppController& appController;
    juce::Label brand { {}, "VMPC2000XL" };
    juce::Label telemetry;
    juce::TextButton playBtn { "Play" };
    juce::TextButton stopBtn { "Stop" };
    juce::TextButton recBtn { "Rec" };
    juce::Slider swingSlider;
};
} // namespace vmpc::view
