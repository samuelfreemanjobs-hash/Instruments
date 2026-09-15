#pragma once

#include <JuceHeader.h>

namespace vmpc::model
{
/** Phase 5: full project persistence via ValueTree. */
class ProjectState
{
public:
    static inline const juce::Identifier rootId { "VMPC2000XL" };
    static inline const juce::Identifier projectId { "Project" };

    ProjectState();

    juce::ValueTree& getTree() noexcept { return project; }
    const juce::ValueTree& getTree() const noexcept { return project; }

    void resetToDefaults();

    juce::File getDefaultProjectFile() const;

private:
    juce::ValueTree project;
};
} // namespace vmpc::model
