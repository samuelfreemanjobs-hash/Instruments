#pragma once

#include <JuceHeader.h>
#include "Model/ProjectState.h"
#include "Model/HybridWorkspace.h"
#include "Audio/AudioEngine.h"
#include "Audio/PluginHostService.h"

namespace vmpc::controller
{
class AppController
{
public:
    struct Listener
    {
        virtual ~Listener() = default;
        virtual void appModeChanged(vmpc::model::AppMode mode) = 0;
    };

    AppController(model::ProjectState& projectState, audio::AudioEngine& engine);

    void attachAudioDeviceManager(juce::AudioDeviceManager& manager);
    void syncProjectToSequencer();

    void setAppMode(model::AppMode mode);
    model::AppMode getAppMode() const noexcept { return workspace.getMode(); }

    model::ProjectState& getProject() noexcept { return project; }
    model::HybridWorkspace& getWorkspace() noexcept { return workspace; }
    audio::AudioEngine& getEngine() noexcept { return audioEngine; }
    audio::PluginHostService& getPluginHost() noexcept { return *pluginHost; }

    void addListener(Listener* listener);
    void removeListener(Listener* listener);

    void persistMixingState();
    bool saveProjectToFile(const juce::File& file);
    bool loadProjectFromFile(const juce::File& file);

private:
    void notifyModeChanged(model::AppMode mode);

    model::ProjectState& project;
    model::HybridWorkspace workspace;
    audio::AudioEngine& audioEngine;
    std::unique_ptr<audio::PluginHostService> pluginHost;
    juce::AudioDeviceManager* deviceManager = nullptr;
    juce::ListenerList<Listener> listeners;
};
} // namespace vmpc::controller
