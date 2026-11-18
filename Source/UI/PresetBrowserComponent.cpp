#include "UI/PresetBrowserComponent.h"

namespace
{
bool matchesSearch (const juce::String& haystack, const juce::String& needle) noexcept
{
    return needle.isEmpty() || haystack.containsIgnoreCase (needle);
}
} // namespace

PresetBrowserComponent::PresetBrowserComponent (CountFn getCount,
                                                NameFn getName,
                                                CategoryFn getCategory,
                                                SelectFn onSelect)
    : getCount_ (std::move (getCount)),
      getName_ (std::move (getName)),
      getCategory_ (std::move (getCategory)),
      onSelect_ (std::move (onSelect))
{
    search_.setTextToShowWhenEmpty ("Search presets…", juce::Colours::grey);
    search_.setFont (juce::Font (13.0f));
    search_.addListener (this);
    addAndMakeVisible (search_);

    categoryFilter_.addItem ("All categories", 1);
    categoryFilter_.setSelectedId (1, juce::dontSendNotification);
    categoryFilter_.addListener (this);
    addAndMakeVisible (categoryFilter_);

    prev_.onClick = [this] { navigateByDelta (-1); };
    next_.onClick = [this] { navigateByDelta (1); };
    addAndMakeVisible (prev_);
    addAndMakeVisible (next_);

    summary_.setJustificationType (juce::Justification::centredLeft);
    summary_.setFont (juce::Font (12.0f, juce::Font::bold));
    addAndMakeVisible (summary_);

    listBox_.setModel (this);
    listBox_.setRowHeight (22);
    listBox_.setColour (juce::ListBox::backgroundColourId, juce::Colour (0xff121218));
    listBox_.setColour (juce::ListBox::outlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible (listBox_);

    rebuildFiltered();
    updateSummary();
}

void PresetBrowserComponent::setCategoryFilterOptions (const juce::StringArray& categories)
{
    categoryFilter_.clear (juce::dontSendNotification);
    categoryFilter_.addItem ("All categories", 1);
    int id = 2;
    for (const auto& c : categories)
        categoryFilter_.addItem (c, id++);
    categoryFilter_.setSelectedId (1, juce::dontSendNotification);
}

void PresetBrowserComponent::setCurrentProgram (int programIndex)
{
    currentProgram_ = juce::jlimit (0, std::max (0, getCount_() - 1), programIndex);
    updateSummary();
    rebuildFiltered();

    for (int row = 0; row < static_cast<int> (filteredPrograms_.size()); ++row)
    {
        if (filteredPrograms_[static_cast<std::size_t> (row)] == currentProgram_)
        {
            listBox_.selectRow (row, false, juce::dontSendNotification);
            listBox_.scrollToEnsureRowIsOnscreen (row);
            break;
        }
    }
}

void PresetBrowserComponent::refresh()
{
    rebuildFiltered();
    updateSummary();
}

void PresetBrowserComponent::resized()
{
    auto area = getLocalBounds();

    auto topRow = area.removeFromTop (26);
    prev_.setBounds (topRow.removeFromLeft (28));
    next_.setBounds (topRow.removeFromLeft (28));
    summary_.setBounds (topRow);

    auto filterRow = area.removeFromTop (26);
    categoryFilter_.setBounds (filterRow.removeFromLeft (juce::jmax (110, filterRow.getWidth() / 2)));
    filterRow.removeFromLeft (6);
    search_.setBounds (filterRow);

    listBox_.setBounds (area);
}

int PresetBrowserComponent::getNumRows()
{
    return static_cast<int> (filteredPrograms_.size());
}

void PresetBrowserComponent::paintListBoxItem (int rowNumber,
                                               juce::Graphics& g,
                                               int width,
                                               int height,
                                               bool rowIsSelected)
{
    if (! juce::isPositiveAndBelow (rowNumber, static_cast<int> (filteredPrograms_.size())))
        return;

    const int program = filteredPrograms_[static_cast<std::size_t> (rowNumber)];
    const auto name = getName_ (program);
    const auto cat = getCategory_ (program);

    if (rowIsSelected)
        g.fillAll (juce::Colour (0xff2e3a52));
    else if (rowNumber % 2 == 0)
        g.fillAll (juce::Colour (0xff16161c));

    g.setColour (juce::Colours::white.withAlpha (0.92f));
    g.setFont (juce::Font (12.0f));
    g.drawText (juce::String (program + 1).paddedLeft ('0', 3) + "  " + name,
                6,
                0,
                width - 90,
                height,
                juce::Justification::centredLeft);

    g.setColour (juce::Colour (0xff9aa0b8));
    g.setFont (juce::Font (11.0f));
    g.drawText (cat, width - 84, 0, 78, height, juce::Justification::centredRight);
}

void PresetBrowserComponent::listBoxItemClicked (int row, const juce::MouseEvent&)
{
    loadSelectionFromRow (row);
}

void PresetBrowserComponent::selectedRowsChanged (int lastRowSelected)
{
    if (suppressCallbacks_ || lastRowSelected < 0)
        return;
    loadSelectionFromRow (lastRowSelected);
}

void PresetBrowserComponent::textEditorTextChanged (juce::TextEditor&)
{
    rebuildFiltered();
}

void PresetBrowserComponent::comboBoxChanged (juce::ComboBox*)
{
    rebuildFiltered();
}

void PresetBrowserComponent::rebuildFiltered()
{
    filteredPrograms_.clear();
    const auto needle = search_.getText().trim();

    const int selectedCat = categoryFilter_.getSelectedId();
    juce::String activeCategory;
    if (selectedCat > 1)
        activeCategory = categoryFilter_.getText();

    const int total = getCount_();
    for (int i = 0; i < total; ++i)
    {
        const auto name = getName_ (i);
        const auto cat = getCategory_ (i);
        if (! matchesSearch (name, needle) && ! matchesSearch (cat, needle))
            continue;
        if (selectedCat > 1 && cat != activeCategory)
            continue;
        filteredPrograms_.push_back (i);
    }

    listBox_.updateContent();
    listBox_.repaint();

    for (int row = 0; row < static_cast<int> (filteredPrograms_.size()); ++row)
    {
        if (filteredPrograms_[static_cast<std::size_t> (row)] == currentProgram_)
        {
            listBox_.selectRow (row, false, juce::dontSendNotification);
            return;
        }
    }
}

void PresetBrowserComponent::navigateByDelta (int delta)
{
    const int total = getCount_();
    if (total <= 0)
        return;

    int next = currentProgram_ + delta;
    next = juce::jlimit (0, total - 1, next);
    if (next == currentProgram_)
        return;

    suppressCallbacks_ = true;
    currentProgram_ = next;
    if (onSelect_)
        onSelect_ (currentProgram_);
    updateSummary();
    rebuildFiltered();
    suppressCallbacks_ = false;
}

void PresetBrowserComponent::loadSelectionFromRow (int row)
{
    if (! juce::isPositiveAndBelow (row, static_cast<int> (filteredPrograms_.size())))
        return;

    const int program = filteredPrograms_[static_cast<std::size_t> (row)];
    if (program == currentProgram_)
        return;

    suppressCallbacks_ = true;
    currentProgram_ = program;
    if (onSelect_)
        onSelect_ (currentProgram_);
    updateSummary();
    suppressCallbacks_ = false;
}

void PresetBrowserComponent::updateSummary()
{
    const int total = getCount_();
    if (total <= 0)
    {
        summary_.setText ("No presets", juce::dontSendNotification);
        return;
    }

    summary_.setText (juce::String (currentProgram_ + 1) + " / " + juce::String (total) + "  ·  "
                          + getName_ (currentProgram_),
                      juce::dontSendNotification);
}
