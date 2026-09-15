#include "ProjectState.h"
#include "Audio/RealTimeConstraints.h"

namespace vmpc::model
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
    project.setProperty("swing", 54, nullptr);
    for (int i = 0; i < 4; ++i)
        project.setProperty("qlink" + juce::String(i), 0.5, nullptr);
    project.setProperty("ppqn", vmpc::audio::kDefaultPpqn, nullptr);
}

juce::File ProjectState::getDefaultProjectFile() const
{
    return juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
        .getChildFile("VMPC2000XL")
        .getChildFile("Untitled.vmpc");
}
} // namespace vmpc::model
