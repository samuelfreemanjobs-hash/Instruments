#include "PresetBrowser.h"
#include "../FactoryLookAndFeel.h"

namespace fds {
namespace gui {

PresetBrowser::PresetBrowser()
{
    for (auto* b : { &prev, &next, &nameButton })
    {
        b->getProperties().set (FactoryLookAndFeel::buttonVariantProperty, "secondary");
        addAndMakeVisible (b);
    }
    // Preset name follows body case (as-is), not legend uppercase — design-spec "Init".
    nameButton.getProperties().set (FactoryLookAndFeel::buttonTextCaseProperty, "none");
    prev.onClick = [this] { if (onPrev) onPrev(); };
    next.onClick = [this] { if (onNext) onNext(); };
    nameButton.onClick = [this] { showMenu(); };
    setPresetName ("Init");
    setSize (220, size::toggle::md);
}

void PresetBrowser::setPresetName (const juce::String& name, bool isModified)
{
    modified = isModified;
    auto shown = name;
    if (modified)
        shown += " *";
    nameButton.setButtonText (shown);
    nameButton.setColour (juce::TextButton::textColourOffId,
                          modified ? argb (color::text::secondary) : argb (color::text::primary));
}

juce::String PresetBrowser::getPresetName() const { return nameButton.getButtonText(); }

void PresetBrowser::setPresetList (juce::StringArray factoryPresets, juce::StringArray userPresets)
{
    factoryList = std::move (factoryPresets);
    userList = std::move (userPresets);
}

void PresetBrowser::showMenu()
{
    juce::PopupMenu m;
    int id = 1;
    for (auto& p : factoryList)
        m.addItem (id++, p);
    if (factoryList.size() > 0 && userList.size() > 0)
        m.addSeparator();
    for (auto& p : userList)
        m.addItem (id++, p);
    m.addSeparator();
    const int saveId = 1000, saveAsId = 1001, renameId = 1002, deleteId = 1003, folderId = 1004;
    m.addItem (saveId, "Save");
    m.addItem (saveAsId, "Save as...");
    m.addItem (renameId, "Rename");
    m.addItem (deleteId, "Delete");
    m.addItem (folderId, "Open folder");

    m.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (&nameButton),
                     [this, saveId, saveAsId, renameId, deleteId, folderId] (int result)
                     {
                         if (result <= 0)
                             return;
                         if (result >= 1000)
                         {
                             juce::String action = "save";
                             if (result == saveAsId) action = "saveAs";
                             else if (result == renameId) action = "rename";
                             else if (result == deleteId) action = "delete";
                             else if (result == folderId) action = "openFolder";
                             if (onMenuAction)
                                 onMenuAction (action);
                             return;
                         }
                         if (onPresetChosen)
                             onPresetChosen (result - 1);
                     });
}

void PresetBrowser::paint (juce::Graphics& g)
{
    g.setColour (argb (color::bg::well));
    g.fillPath (wellPath);
}

void PresetBrowser::resized()
{
    wellPath.clear();
    wellPath.addRoundedRectangle (getLocalBounds().toFloat(), radius::xs);
    auto r = getLocalBounds();
    prev.setBounds (r.removeFromLeft (24));
    next.setBounds (r.removeFromRight (24));
    nameButton.setBounds (r);
}

} // namespace gui
} // namespace fds
