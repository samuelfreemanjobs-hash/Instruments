#include "AppController.h"
#include "Audio/RealTimeConstraints.h"

namespace vmpc::controller
{
AppController::AppController(model::ProjectState& projectState, audio::AudioEngine& engine)
    : project(projectState)
    , audioEngine(engine)
{
    audioEngine.bindElectribeSong(&workspace.getElectribeSong());
    pluginHost = std::make_unique<audio::PluginHostService>(audioEngine);
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
} // namespace vmpc::controller
