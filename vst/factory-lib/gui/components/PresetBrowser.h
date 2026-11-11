#pragma once

#include "../GuiHelpers.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace fds {
namespace gui {

class PresetBrowser : public juce::Component
{
public:
    PresetBrowser();

    void setPresetName (const juce::String& name, bool modified = false);
    juce::String getPresetName() const;
    void setPresetList (juce::StringArray factoryPresets, juce::StringArray userPresets = {});

    std::function<void (int index)> onPresetChosen;
    std::function<void()> onPrev, onNext;
    std::function<void (const juce::String& action)> onMenuAction;

    /** Focus targets for product tab-order (‹ → name → ›). */
    juce::TextButton& getPrevButton() noexcept { return prev; }
    juce::TextButton& getNextButton() noexcept { return next; }
    juce::TextButton& getNameButton() noexcept { return nameButton; }

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void showMenu();

    juce::TextButton prev { prevGlyph() };
    juce::TextButton next { nextGlyph() };
    juce::TextButton nameButton;
    juce::StringArray factoryList, userList;
    juce::Path wellPath;
    bool modified = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetBrowser)
};

} // namespace gui
} // namespace fds
