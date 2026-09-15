#include "AppController.h"
#include "Audio/RealTimeConstraints.h"
#include "Model/MixingProjectPersistence.h"

namespace vmpc::controller
{
AppController::AppController(model::ProjectState& projectState, audio::AudioEngine& engine)
    : project(projectState)
    , audioEngine(engine)
{
    audioEngine.bindElectribeSong(&workspace.getElectribeSong());
    pluginHost = std::make_unique<audio::PluginHostService>(audioEngine);
    pluginHost->setMixingChangedCallback([this]() { persistMixingState(); });
    model::readMixingFromProject(project.getTree(), *pluginHost);
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

void AppController::persistMixingState()
{
    model::writeMixingToProject(project.getTree(), *pluginHost);
}

bool AppController::saveProjectToFile(const juce::File& file)
{
    persistMixingState();
    if (auto xml = project.getTree().createXml())
        return xml->writeTo(file);
    return false;
}

bool AppController::loadProjectFromFile(const juce::File& file)
{
    if (auto xml = juce::parseXML(file))
    {
        project.getTree() = juce::ValueTree::fromXml(*xml);
        syncProjectToSequencer();
        model::readMixingFromProject(project.getTree(), *pluginHost);
        return true;
    }
    return false;
}
} // namespace vmpc::controller
