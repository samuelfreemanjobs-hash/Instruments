#pragma once

#include <JuceHeader.h>
#include "Audio/VibeMixingTypes.h"

namespace resonance::view
{
/** Modal preview before applying a vibe recipe (Apply / Cancel, variants). */
class VibeMixPreviewDialog : public juce::Component
{
public:
    using ApplyCallback = std::function<void(const resonance::audio::VibeMixRecipe&)>;

    VibeMixPreviewDialog(resonance::audio::VibeMixInterpretResult result, ApplyCallback onApply);

    void paint(juce::Graphics& g) override;
    void resized() override;

    static void show(resonance::audio::VibeMixInterpretResult result, ApplyCallback onApply);

private:
    void setActiveRecipe(const resonance::audio::VibeMixRecipe& recipe);
    void rebuildSlotList();
    juce::String formatConflicts() const;

    resonance::audio::VibeMixInterpretResult interpretResult;
    resonance::audio::VibeMixRecipe activeRecipe;
    ApplyCallback applyCallback;

    juce::Label titleLabel;
    juce::Label explanationLabel;
    juce::Label conflictsLabel;
    juce::ListBox slotList { "slots", nullptr };
    juce::TextButton mainVariant { "Main preset" };
    juce::TextButton brighterButton { "Brighter" };
    juce::TextButton darkerButton { "Darker" };
    juce::TextButton applyButton { "Apply vibe" };
    juce::TextButton cancelButton { "Cancel" };

    juce::StringArray slotLines;
    std::unique_ptr<juce::ListBoxModel> slotListModel;
};
} // namespace resonance::view
