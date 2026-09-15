#include "AppController.h"
#include "Audio/RealTimeConstraints.h"

namespace vmpc::controller
{
AppController::AppController(model::ProjectState& projectState, audio::AudioEngine& engine)
    : project(projectState)
    , audioEngine(engine)
{
    syncProjectToSequencer();
}

void AppController::attachAudioDeviceManager(juce::AudioDeviceManager& manager)
{
    deviceManager = &manager;
    manager.addAudioCallback(&audioEngine);
}

void AppController::syncProjectToSequencer()
{
    auto& seq = audioEngine.getSequencer();
    const auto& tree = project.getTree();
    seq.setBpm(tree.getProperty("bpm", 92.0));
    seq.setSwing(static_cast<int>(tree.getProperty("swing", 54)));
    seq.setPpqn(static_cast<int>(tree.getProperty("ppqn", vmpc::audio::kDefaultPpqn)));
}
} // namespace vmpc::controller
