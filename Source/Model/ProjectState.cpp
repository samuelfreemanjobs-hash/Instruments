#include "ProjectState.h"
#include "Audio/RealTimeConstraints.h"

namespace resonance::model
{
ProjectState::ProjectState()
{
    resetToDefaults();
}

void ProjectState::resetToDefaults()
{
    project = juce::ValueTree(projectId);
    project.setProperty("version", 1, nullptr);
    project.setProperty("appMode", "Electribe", nullptr);
    project.setProperty("bpm", 120.0, nullptr);
    project.setProperty("swing", 50, nullptr);
    project.setProperty("ppqn", resonance::audio::kDefaultPpqn, nullptr);
}

juce::File ProjectState::getDefaultProjectFile() const
{
    return juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
        .getChildFile("Resonance")
        .getChildFile("Untitled.resonance");
}
} // namespace resonance::model
