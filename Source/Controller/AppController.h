#pragma once

#include <JuceHeader.h>
#include "Model/ProjectState.h"
#include "Model/HybridWorkspace.h"
#include "Audio/AudioEngine.h"

namespace vmpc::controller
{
class AppController
{
public:
    struct Listener
    {
        virtual ~Listener() = default;
        virtual void appModeChanged(vmpc::model::AppMode mode) = 0;
        virtual void transportStateChanged(bool playing) {}
    };

    AppController(model::ProjectState& projectState, audio::AudioEngine& engine);

    void attachAudioDeviceManager(juce::AudioDeviceManager& manager);
    void syncProjectToSequencer();

    void setQLinkValue(int linkIndex, float value01);

    void setAppMode(model::AppMode mode);
    model::AppMode getAppMode() const noexcept { return workspace.getMode(); }

    void setTransportPlaying(bool playing);
    bool isTransportPlaying() const noexcept;
    void toggleTransport();

    model::ProjectState& getProject() noexcept { return project; }
    model::HybridWorkspace& getWorkspace() noexcept { return workspace; }
    audio::AudioEngine& getEngine() noexcept { return audioEngine; }

    void addListener(Listener* listener);
    void removeListener(Listener* listener);

private:
    void notifyModeChanged(model::AppMode mode);
    void notifyTransport(bool playing);

    model::ProjectState& project;
    model::HybridWorkspace workspace;
    audio::AudioEngine& audioEngine;
    juce::AudioDeviceManager* deviceManager = nullptr;
    juce::ListenerList<Listener> listeners;
};
} // namespace vmpc::controller
