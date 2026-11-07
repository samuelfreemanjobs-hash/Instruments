#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>
#include <vector>

/** Searchable factory preset strip for large program banks (200+). */
class PresetBrowserComponent final : public juce::Component,
                                     private juce::ListBoxModel,
                                     private juce::TextEditor::Listener,
                                     private juce::ComboBox::Listener
{
public:
    using CountFn = std::function<int()>;
    using NameFn = std::function<juce::String (int index)>;
    using CategoryFn = std::function<juce::String (int index)>;
    using SelectFn = std::function<void (int index)>;

    PresetBrowserComponent (CountFn getCount, NameFn getName, CategoryFn getCategory, SelectFn onSelect);

    void setCategoryFilterOptions (const juce::StringArray& categories);
    void setCurrentProgram (int programIndex);
    void refresh();

    void resized() override;

private:
    int getNumRows() override;
    void paintListBoxItem (int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked (int row, const juce::MouseEvent&) override;
    void selectedRowsChanged (int lastRowSelected) override;

    void textEditorTextChanged (juce::TextEditor&) override;
    void comboBoxChanged (juce::ComboBox*) override;

    void rebuildFiltered();
    void navigateByDelta (int delta);
    void loadSelectionFromRow (int row);
    void updateSummary();

    CountFn getCount_;
    NameFn getName_;
    CategoryFn getCategory_;
    SelectFn onSelect_;

    juce::ListBox listBox_;
    juce::TextEditor search_;
    juce::ComboBox categoryFilter_;
    juce::TextButton prev_ { "<" };
    juce::TextButton next_ { ">" };
    juce::Label summary_ { {}, "Presets" };

    std::vector<int> filteredPrograms_;
    int currentProgram_ = 0;
    bool suppressCallbacks_ = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetBrowserComponent)
};
