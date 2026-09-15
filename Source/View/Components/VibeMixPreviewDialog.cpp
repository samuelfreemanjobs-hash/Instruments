#include "VibeMixPreviewDialog.h"

namespace resonance::view
{
namespace
{
class SlotListModel : public juce::ListBoxModel
{
public:
    explicit SlotListModel(juce::StringArray& lines)
        : linesRef(lines)
    {
    }

    int getNumRows() override { return linesRef.size(); }

    void paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool rowIsSelected) override
    {
        if (!juce::isPositiveAndBelow(row, linesRef.size()))
            return;

        if (rowIsSelected)
            g.fillAll(juce::Colour(0xff1e3a5f));

        g.setColour(juce::Colours::white.withAlpha(0.92f));
        g.setFont(13.0f);
        g.drawText(linesRef[row], 6, 0, width - 8, height, juce::Justification::centredLeft, true);
    }

private:
    juce::StringArray& linesRef;
};
} // namespace

VibeMixPreviewDialog::VibeMixPreviewDialog(resonance::audio::VibeMixInterpretResult result, ApplyCallback onApply)
    : interpretResult(std::move(result))
    , applyCallback(std::move(onApply))
{
    titleLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xff38bdf8));
    addAndMakeVisible(titleLabel);

    explanationLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible(explanationLabel);

    conflictsLabel.setColour(juce::Label::textColourId, juce::Colour(0xfff59e0b));
    addAndMakeVisible(conflictsLabel);

    slotListModel = std::make_unique<SlotListModel>(slotLines);
    slotList.setModel(slotListModel.get());
    addAndMakeVisible(slotList);

    const auto pickVariant = [this](const resonance::audio::VibeMixRecipe& recipe) { setActiveRecipe(recipe); };

    mainVariant.onClick = [this, pickVariant]() { pickVariant(interpretResult.recipe); };
    brighterButton.onClick = [this, pickVariant]() {
        if (interpretResult.brighterVariant.has_value())
            pickVariant(*interpretResult.brighterVariant);
    };
    darkerButton.onClick = [this, pickVariant]() {
        if (interpretResult.darkerVariant.has_value())
            pickVariant(*interpretResult.darkerVariant);
    };

    applyButton.onClick = [this]() {
        if (applyCallback)
            applyCallback(activeRecipe);
        if (auto* dw = findParentComponentOfClass<juce::DialogWindow>())
            dw->exitModalState(1);
    };

    cancelButton.onClick = [this]() {
        if (auto* dw = findParentComponentOfClass<juce::DialogWindow>())
            dw->exitModalState(0);
    };

    for (auto* b : { &mainVariant, &brighterButton, &darkerButton, &applyButton, &cancelButton })
        addAndMakeVisible(*b);

    brighterButton.setEnabled(interpretResult.brighterVariant.has_value());
    darkerButton.setEnabled(interpretResult.darkerVariant.has_value());

    setActiveRecipe(interpretResult.recipe);
    conflictsLabel.setText(formatConflicts(), juce::dontSendNotification);

    setSize(520, 420);
}

void VibeMixPreviewDialog::setActiveRecipe(const resonance::audio::VibeMixRecipe& recipe)
{
    activeRecipe = recipe;
    titleLabel.setText(recipe.title, juce::dontSendNotification);
    explanationLabel.setText(recipe.explanation, juce::dontSendNotification);
    rebuildSlotList();
}

void VibeMixPreviewDialog::rebuildSlotList()
{
    slotLines.clear();
    for (const auto& slot : activeRecipe.slots)
    {
        juce::String pluginName;
        for (const auto& info : resonance::audio::internal::allMixPlugins())
        {
            if (info.id == slot.plugin)
            {
                pluginName = info.displayName;
                break;
            }
        }

        slotLines.add("Slot " + juce::String(slot.slotIndex + 1) + ": " + pluginName + " — " + slot.rationale);
    }
    slotList.updateContent();
}

juce::String VibeMixPreviewDialog::formatConflicts() const
{
    if (interpretResult.recipe.conflicts.empty())
        return {};

    juce::String out = "Note: ";
    for (size_t i = 0; i < interpretResult.recipe.conflicts.size(); ++i)
    {
        if (i > 0)
            out += " ";
        out += interpretResult.recipe.conflicts[i];
    }
    return out;
}

void VibeMixPreviewDialog::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff111318));
}

void VibeMixPreviewDialog::resized()
{
    auto area = getLocalBounds().reduced(12);
    titleLabel.setBounds(area.removeFromTop(24));
    explanationLabel.setBounds(area.removeFromTop(40));
    conflictsLabel.setBounds(area.removeFromTop(36));
    area.removeFromTop(6);

    auto variantRow = area.removeFromTop(28);
    const int w = variantRow.getWidth() / 3;
    mainVariant.setBounds(variantRow.removeFromLeft(w).reduced(2));
    brighterButton.setBounds(variantRow.removeFromLeft(w).reduced(2));
    darkerButton.setBounds(variantRow.reduced(2));

    area.removeFromTop(6);
    auto buttons = area.removeFromBottom(32);
    cancelButton.setBounds(buttons.removeFromRight(100).reduced(2));
    applyButton.setBounds(buttons.removeFromRight(120).reduced(2));

    slotList.setBounds(area);
}

void VibeMixPreviewDialog::show(resonance::audio::VibeMixInterpretResult result, ApplyCallback onApply)
{
    if (!result.success)
        return;

    auto* content = new VibeMixPreviewDialog(std::move(result), std::move(onApply));

    juce::DialogWindow::LaunchOptions opts;
    opts.dialogTitle = "Vibe Mix preview";
    opts.dialogBackgroundColour = juce::Colour(0xff111318);
    opts.content.setOwned(content);
    opts.componentToCentreAround = nullptr;
    opts.useNativeTitleBar = true;
    opts.resizable = false;
    opts.launchAsync();
}
} // namespace resonance::view
