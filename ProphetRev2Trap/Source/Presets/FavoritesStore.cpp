#include "Presets/FavoritesStore.h"

namespace prophetrev2::presets
{

namespace
{
constexpr const char* kRootType = "NightCircuitFavorites";
constexpr const char* kFactoryType = "factory";
constexpr const char* kUserType = "user";
} // namespace

juce::File FavoritesStore::storeFile()
{
    return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
        .getChildFile ("Instruments")
        .getChildFile ("NightCircuit")
        .getChildFile ("favorites.xml");
}

juce::ValueTree FavoritesStore::loadTree()
{
    const auto file = storeFile();
    if (file.existsAsFile())
        if (auto xml = juce::XmlDocument::parse (file))
            return juce::ValueTree::fromXml (*xml);

    return juce::ValueTree (kRootType);
}

bool FavoritesStore::saveTree (const juce::ValueTree& root)
{
    const auto file = storeFile();
    if (file.getParentDirectory().createDirectory().failed())
        return false;
    if (auto xml = root.createXml())
        return xml->writeTo (file);
    return false;
}

bool FavoritesStore::isFactoryFavorite (int factoryGlobalIndex)
{
    if (factoryGlobalIndex < 0)
        return false;

    const auto root = loadTree();
    for (int i = 0; i < root.getNumChildren(); ++i)
    {
        const auto child = root.getChild (i);
        if (child.hasType (kFactoryType)
            && static_cast<int> (child.getProperty ("index", -1)) == factoryGlobalIndex)
            return true;
    }
    return false;
}

void FavoritesStore::setFactoryFavorite (int factoryGlobalIndex, bool favorite)
{
    if (factoryGlobalIndex < 0)
        return;

    auto root = loadTree();
    if (! root.hasType (kRootType))
        root = juce::ValueTree (kRootType);

    for (int i = root.getNumChildren(); --i >= 0;)
    {
        const auto child = root.getChild (i);
        if (child.hasType (kFactoryType)
            && static_cast<int> (child.getProperty ("index", -1)) == factoryGlobalIndex)
            root.removeChild (i, nullptr);
    }

    if (favorite)
    {
        juce::ValueTree entry (kFactoryType);
        entry.setProperty ("index", factoryGlobalIndex, nullptr);
        root.appendChild (entry, nullptr);
    }

    saveTree (root);
}

bool FavoritesStore::isUserFavorite (const juce::String& presetName)
{
    if (presetName.isEmpty())
        return false;

    const auto root = loadTree();
    for (int i = 0; i < root.getNumChildren(); ++i)
    {
        const auto child = root.getChild (i);
        if (child.hasType (kUserType) && child.getProperty ("name").toString() == presetName)
            return true;
    }
    return false;
}

void FavoritesStore::setUserFavorite (const juce::String& presetName, bool favorite)
{
    if (presetName.isEmpty())
        return;

    auto root = loadTree();
    if (! root.hasType (kRootType))
        root = juce::ValueTree (kRootType);

    for (int i = root.getNumChildren(); --i >= 0;)
    {
        const auto child = root.getChild (i);
        if (child.hasType (kUserType) && child.getProperty ("name").toString() == presetName)
            root.removeChild (i, nullptr);
    }

    if (favorite)
    {
        juce::ValueTree entry (kUserType);
        entry.setProperty ("name", presetName, nullptr);
        root.appendChild (entry, nullptr);
    }

    saveTree (root);
}

} // namespace prophetrev2::presets
