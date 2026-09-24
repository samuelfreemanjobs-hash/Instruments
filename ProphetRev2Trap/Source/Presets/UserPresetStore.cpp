#include "Presets/UserPresetStore.h"

namespace prophetrev2::presets
{

juce::File UserPresetStore::presetsDirectory()
{
    auto dir = juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
                   .getChildFile ("Instruments")
                   .getChildFile ("NightCircuit")
                   .getChildFile ("Presets");
    dir.createDirectory();
    return dir;
}

juce::String UserPresetStore::sanitiseName (const juce::String& name)
{
    auto s = name.trim();
    if (s.isEmpty())
        s = "Untitled";
    return s.replaceCharacter ('/', '_').replaceCharacter ('\\', '_');
}

juce::File UserPresetStore::fileForName (const juce::String& name)
{
    return presetsDirectory().getChildFile (sanitiseName (name) + ".nightcircuit.xml");
}

juce::StringArray UserPresetStore::listPresetNames()
{
    juce::StringArray names;
    for (const auto& f : presetsDirectory().findChildFiles (juce::File::findFiles, false, "*.nightcircuit.xml"))
        names.add (f.getFileNameWithoutExtension());
    names.sort (true);
    return names;
}

bool UserPresetStore::savePreset (const juce::String& name, const juce::ValueTree& apvtsState)
{
    const auto file = fileForName (name);
    if (auto xml = apvtsState.createXml())
        return xml->writeTo (file);
    return false;
}

bool UserPresetStore::loadPreset (const juce::String& name, juce::ValueTree& apvtsStateOut)
{
    const auto file = fileForName (name);
    if (! file.existsAsFile())
        return false;
    if (auto xml = juce::XmlDocument::parse (file))
    {
        apvtsStateOut = juce::ValueTree::fromXml (*xml);
        return apvtsStateOut.isValid();
    }
    return false;
}

bool UserPresetStore::deletePreset (const juce::String& name)
{
    return fileForName (name).deleteFile();
}

} // namespace prophetrev2::presets
