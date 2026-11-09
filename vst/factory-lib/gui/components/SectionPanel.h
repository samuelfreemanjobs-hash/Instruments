#pragma once

#include "../GuiHelpers.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace fds {
namespace gui {

class SectionPanel : public juce::Component
{
public:
    explicit SectionPanel (const juce::String& titleText = {});

    void setTitle (const juce::String& titleText);
    juce::Rectangle<int> getContentArea() const;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Label title;
    juce::Path bodyPath;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SectionPanel)
};

} // namespace gui
} // namespace fds
