#include "PresetManager.h"

namespace characterfm::presets
{

juce::String exportPresetJson (const juce::ValueTree& state, const juce::String& presetName)
{
    juce::DynamicObject::Ptr root = new juce::DynamicObject();
    root->setProperty ("format", "DISKLORDZ_CHARACTER_FM_PRESET");
    root->setProperty ("version", 1);
    root->setProperty ("name", presetName);

    juce::DynamicObject::Ptr fm = new juce::DynamicObject();
    fm->setProperty ("apvtsXml", state.toXmlString());
    root->setProperty ("fm", juce::var (fm.get()));

    return juce::JSON::toString (juce::var (root.get()), true);
}

bool importPresetJson (const juce::String& json, juce::ValueTree& targetState)
{
    const auto parsed = juce::JSON::parse (json);
    if (! parsed.isObject())
        return false;

    const auto* root = parsed.getDynamicObject();
    if (root == nullptr)
        return false;

    if (root->getProperty ("format").toString() != "DISKLORDZ_CHARACTER_FM_PRESET")
        return false;

    const int version = static_cast<int> (root->getProperty ("version"));
    if (version != 1)
        return false;

    const auto fmVar = root->getProperty ("fm");
    if (! fmVar.isObject())
        return false;

    const auto* fm = fmVar.getDynamicObject();
    if (fm == nullptr)
        return false;

    const auto xml = fm->getProperty ("apvtsXml").toString();
    if (auto doc = juce::XmlDocument::parse (xml))
    {
        targetState = juce::ValueTree::fromXml (*doc);
        return targetState.isValid();
    }

    return false;
}

} // namespace characterfm::presets
