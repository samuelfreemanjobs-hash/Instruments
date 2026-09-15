#pragma once

#include <JuceHeader.h>
#include "Model/ProjectState.h"
#include "Audio/AudioEngine.h"

namespace vmpc::controller
{
/** Message-thread only: connects views to model and audio device setup. */
class AppController
{
public:
    AppController(model::ProjectState& projectState, audio::AudioEngine& engine);

    void attachAudioDeviceManager(juce::AudioDeviceManager& manager);
    void syncProjectToSequencer();

    model::ProjectState& getProject() noexcept { return project; }
    audio::AudioEngine& getEngine() noexcept { return audioEngine; }

private:
    model::ProjectState& project;
    audio::AudioEngine& audioEngine;
    juce::AudioDeviceManager* deviceManager = nullptr;
};
} // namespace vmpc::controller
