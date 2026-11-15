#include "PresetVaultBrowser.h"

#include "CelestialTheme.h"
#include "PluginProcessor.h"
#include "Presets/PresetParams.h"

namespace junovax::ui::celestial
{
PresetVaultBrowser::PresetVaultBrowser (JunovaXAudioProcessor& processor, std::function<void()> onClose)
    : processor_ (processor), onClose_ (std::move (onClose))
{
    setVisible (false);
    search_.setMultiLine (false);
    search_.setTextToShowWhenEmpty ("Search patches…", Theme::textMuted());
    search_.setColour (juce::TextEditor::backgroundColourId, Theme::panelGrey());
    search_.setColour (juce::TextEditor::textColourId, Theme::specularChrome());
    search_.addListener (this);
    addAndMakeVisible (search_);

    list_.setModel (this);
    list_.setColour (juce::ListBox::backgroundColourId, Theme::panelGrey());
    addAndMakeVisible (list_);

    categoryFilter_.addItem ("ALL PRESETS", 1);
    categoryFilter_.addItem ("PADS", 2);
    categoryFilter_.addItem ("BASS", 3);
    categoryFilter_.addItem ("LEAD", 4);
    categoryFilter_.addItem ("ARP", 5);
    categoryFilter_.addItem ("PLUCKS", 6);
    categoryFilter_.addItem ("FX", 7);
    categoryFilter_.addItem ("106 CLASSICS", 8);
    categoryFilter_.addItem ("TRAP / PHONK", 9);
    categoryFilter_.setSelectedId (1, juce::dontSendNotification);
    categoryFilter_.onChange = [this] { rebuildFilteredIndices(); };
    addAndMakeVisible (categoryFilter_);

    inspectorTitle_.setFont (Theme::headerFont (14.0f));
    inspectorTitle_.setColour (juce::Label::textColourId, Theme::electricCyan());
    inspectorBody_.setJustificationType (juce::Justification::topLeft);
    inspectorBody_.setColour (juce::Label::textColourId, Theme::textMuted());
    addAndMakeVisible (inspectorTitle_);
    addAndMakeVisible (inspectorBody_);

    loadBtn_.onClick = [this]
    {
        if (selectedRow_ >= 0 && selectedRow_ < static_cast<int> (filtered_.size()))
        {
            processor_.setCurrentProgram (filtered_[static_cast<std::size_t> (selectedRow_)]);
            hideOverlay();
        }
    };
    addAndMakeVisible (loadBtn_);

    closeBtn_.onClick = [this] { hideOverlay(); };
    addAndMakeVisible (closeBtn_);

    rebuildFilteredIndices();
}

void PresetVaultBrowser::showOverlay()
{
    setVisible (true);
    toFront (true);
    rebuildFilteredIndices();
}

void PresetVaultBrowser::hideOverlay()
{
    setVisible (false);
    if (onClose_)
        onClose_();
}

void PresetVaultBrowser::rebuildFilteredIndices()
{
    const auto& presets = junovax::presets::getFactoryPresets();
    const auto q = search_.getText().trim().toLowerCase();
    const int cat = categoryFilter_.getSelectedId();

    auto matchesCategory = [cat] (const juce::String& c) -> bool
    {
        if (cat == 1)
            return true;
        const auto u = c.toUpperCase();
        switch (cat)
        {
            case 2:
                return u.contains ("PAD");
            case 3:
                return u == "BASS";
            case 4:
                return u == "LEAD";
            case 5:
                return u.contains ("ARP");
            case 6:
                return u.contains ("PLUCK") || u.contains ("KEY");
            case 7:
                return u.contains ("FX") || u.contains ("SCI");
            case 8:
                return u.contains ("106") || u.contains ("CLASSIC") || u.contains ("JUNO");
            case 9:
                return u.contains ("TRAP") || u.contains ("PHONK");
            default:
                return true;
        }
    };

    filtered_.clear();
    for (int i = 0; i < static_cast<int> (presets.size()); ++i)
    {
        const auto& p = presets[static_cast<std::size_t> (i)];
        const juce::String name (p.name.data());
        const juce::String category (p.category.data());
        if (! matchesCategory (category))
            continue;
        if (q.isNotEmpty() && ! name.toLowerCase().contains (q))
            continue;
        filtered_.push_back (i);
    }
    list_.updateContent();
    selectedRow_ = juce::jlimit (0, juce::jmax (0, static_cast<int> (filtered_.size()) - 1), selectedRow_);
    list_.repaint();

    if (! filtered_.empty())
    {
        const auto& p = presets[static_cast<std::size_t> (filtered_[static_cast<std::size_t> (selectedRow_)])];
        inspectorBody_.setText ("Patch: " + juce::String (p.name.data()) + "\nCategory: " + juce::String (p.category.data())
                                    + "\n\nClassic Juno-106 poly voice with BBD chorus and 24dB OTA ladder.",
                                juce::dontSendNotification);
    }
}

int PresetVaultBrowser::getNumRows()
{
    return static_cast<int> (filtered_.size());
}

void PresetVaultBrowser::paintListBoxItem (int row, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    if (row < 0 || row >= static_cast<int> (filtered_.size()))
        return;
    const auto& presets = junovax::presets::getFactoryPresets();
    const auto& p = presets[static_cast<std::size_t> (filtered_[static_cast<std::size_t> (row)])];
    if (rowIsSelected)
    {
        g.setColour (Theme::electricCyan().withAlpha (0.15f));
        g.fillRect (0, 0, width, height);
    }
    g.setColour (rowIsSelected ? Theme::electricCyan() : Theme::specularChrome());
    g.setFont (Theme::readoutFont (12.0f));
    g.drawText (juce::String (p.name.data()), 8, 0, width - 16, height, juce::Justification::centredLeft);
    g.setColour (Theme::textMuted());
    g.drawText (juce::String (p.category.data()), width - 88, 0, 80, height, juce::Justification::centredRight);
}

void PresetVaultBrowser::listBoxItemClicked (int row, const juce::MouseEvent&)
{
    selectedRow_ = row;
    rebuildFilteredIndices();
}

void PresetVaultBrowser::textEditorTextChanged (juce::TextEditor&) { rebuildFilteredIndices(); }
void PresetVaultBrowser::textEditorReturnKeyPressed (juce::TextEditor&) { loadBtn_.triggerClick(); }
void PresetVaultBrowser::textEditorEscapeKeyPressed (juce::TextEditor&) { hideOverlay(); }

void PresetVaultBrowser::paint (juce::Graphics& g)
{
    g.fillAll (Theme::background().withAlpha (0.92f));
    g.setColour (Theme::electricCyan());
    g.setFont (Theme::headerFont (22.0f));
    g.drawText ("PRESET VAULT & BROWSER", 24, 16, 400, 28, juce::Justification::centredLeft);
    g.setColour (Theme::textMuted());
    g.setFont (Theme::readoutFont (11.0f));
    g.drawText (juce::String (processor_.getNumPrograms()) + " Patches · MN3009 BBD Calibrated",
                24,
                44,
                400,
                16,
                juce::Justification::centredLeft);
}

void PresetVaultBrowser::resized()
{
    const int pad = 20;
    auto area = getLocalBounds().reduced (pad);
    closeBtn_.setBounds (area.removeFromRight (36).removeFromTop (28));
    search_.setBounds (area.removeFromTop (32).removeFromLeft (juce::jmin (420, area.getWidth() - 200)));
    categoryFilter_.setBounds (area.removeFromTop (32));
    area.removeFromTop (8);

    auto inspector = area.removeFromRight (juce::jmin (280, area.getWidth() / 3));
    list_.setBounds (area);
    inspectorTitle_.setBounds (inspector.removeFromTop (24));
    inspector.removeFromTop (8);
    inspectorBody_.setBounds (inspector.removeFromTop (inspector.getHeight() - 40));
    loadBtn_.setBounds (inspector.removeFromBottom (32));
}

juce::String PresetVaultBrowser::categoryForIndex (int) const
{
    return {};
}
} // namespace junovax::ui::celestial