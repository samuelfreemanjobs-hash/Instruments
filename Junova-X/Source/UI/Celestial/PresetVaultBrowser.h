#pragma once

#include <JuceHeader.h>

class JunovaXAudioProcessor;

namespace junovax::ui::celestial
{
class PresetVaultBrowser final : public juce::Component,
                                 public juce::TextEditor::Listener,
                                 public juce::ListBoxModel
{
public:
    PresetVaultBrowser (JunovaXAudioProcessor& processor, std::function<void()> onClose);

    void paint (juce::Graphics& g) override;
    void resized() override;

    void showOverlay();
    void hideOverlay();

    int getNumRows() override;
    void paintListBoxItem (int row, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked (int row, const juce::MouseEvent&) override;

    void textEditorTextChanged (juce::TextEditor&) override;
    void textEditorReturnKeyPressed (juce::TextEditor&) override;
    void textEditorEscapeKeyPressed (juce::TextEditor&) override;

private:
    void rebuildFilteredIndices();
    juce::String categoryForIndex (int presetIndex) const;

    JunovaXAudioProcessor& processor_;
    std::function<void()> onClose_;

    juce::TextEditor search_;
    juce::ListBox list_;
    juce::TextButton closeBtn_ { "X" };
    juce::ComboBox categoryFilter_;
    juce::Label inspectorTitle_ { {}, "ACTIVE PATCH INSPECTOR" };
    juce::Label inspectorBody_ { {}, {} };
    juce::TextButton loadBtn_ { "LOAD PATCH" };
    juce::TextButton favouriteBtn_ { "FAV" };

    std::vector<int> filtered_;
    int selectedRow_ = 0;
};
} // namespace junovax::ui::celestial