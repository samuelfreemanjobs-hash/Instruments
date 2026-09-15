#include "AppController.h"
#include "Audio/RealTimeConstraints.h"

namespace vmpc::controller
{
AppController::AppController(model::ProjectState& projectState, audio::AudioEngine& engine)
    : project(projectState)
    , audioEngine(engine)
{
    audioEngine.bindElectribeSong(&workspace.getElectribeSong());
    syncProjectToSequencer();

    const auto mode = model::appModeFromString(project.getTree().getProperty("appMode", "Electribe").toString());
    setAppMode(mode);
}

void AppController::attachAudioDeviceManager(juce::AudioDeviceManager& manager)
{
    deviceManager = &manager;
    manager.addAudioCallback(&audioEngine);
}

void AppController::syncProjectToSequencer()
{
    const auto& tree = project.getTree();
    const double bpm = tree.getProperty("bpm", 120.0);
    const int swing = static_cast<int>(tree.getProperty("swing", 50));
    const int ppqn = static_cast<int>(tree.getProperty("ppqn", vmpc::audio::kDefaultPpqn));

    audioEngine.getSequencer().setBpm(bpm);
    audioEngine.getSequencer().setSwing(swing);
    audioEngine.getSequencer().setPpqn(ppqn);

    audioEngine.getElectribeSequencer().setBpm(bpm);
    audioEngine.getElectribeSequencer().setSwing(swing);

    for (int i = 0; i < 4; ++i)
    {
        const auto key = "qlink" + juce::String(i);
        const float v = static_cast<float>(tree.getProperty(key, 0.5));
        juce::ignoreUnused(v);
    }
}

void AppController::setQLinkValue(int linkIndex, float value01)
{
    if (linkIndex < 0 || linkIndex >= 4)
        return;

    const auto key = "qlink" + juce::String(linkIndex);
    project.getTree().setProperty(key, juce::jlimit(0.0f, 1.0f, value01), nullptr);
}

void AppController::setAppMode(model::AppMode mode)
{
    workspace.applyModeDefaults(mode);
    audioEngine.setAppMode(mode);
    project.getTree().setProperty("appMode", model::appModeToString(mode), nullptr);
    notifyModeChanged(mode);
}

void AppController::addListener(Listener* listener)
{
    listeners.add(listener);
}

void AppController::removeListener(Listener* listener)
{
    listeners.remove(listener);
}

void AppController::notifyModeChanged(model::AppMode mode)
{
    listeners.call([mode](Listener& l) { l.appModeChanged(mode); });
}

void AppController::setTransportPlaying(bool playing)
{
    audioEngine.getElectribeSequencer().setTransportRunning(playing);
    notifyTransport(playing);
}

bool AppController::isTransportPlaying() const noexcept
{
    return audioEngine.getElectribeSequencer().isTransportRunning();
}

void AppController::toggleTransport()
{
    setTransportPlaying(!isTransportPlaying());
}

void AppController::notifyTransport(bool playing)
{
    listeners.call([playing](Listener& l) { l.transportStateChanged(playing); });
}
} // namespace vmpc::controller
