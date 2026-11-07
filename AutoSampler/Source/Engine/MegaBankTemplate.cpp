#include "MegaBankTemplate.h"

namespace autosampler
{

bool writeMegaBankJsonTemplate (const juce::File& jsonFile, juce::String& error)
{
    juce::DynamicObject::Ptr root = new juce::DynamicObject();
    root->setProperty ("format", "instruments-autosampler-mega-bank-v2");
    root->setProperty ("name", "OvernightMegaBank");
    root->setProperty ("capture", juce::JSON::parse (R"({
      "lowNote": 36,
      "highNote": 96,
      "noteStep": 3,
      "noteMapPreset": "minorThird",
      "blackKeyPolicy": "skip",
      "recordUntilSilence": true,
      "maxRecordSeconds": 20,
      "multiVelocityLayers": true,
      "velocityLayers": [32, 64, 96, 127],
      "sustainLoop": true
    })"));
    root->setProperty ("run", juce::JSON::parse (R"({
      "failFast": true,
      "skipFailedSources": true,
      "resume": true,
      "writeProgress": true,
      "maxRetriesPerSource": 0,
      "sourceTimeoutSeconds": 3600
    })"));
    root->setProperty ("sources", juce::Array<juce::var>());

    if (! jsonFile.replaceWithText (juce::JSON::toString (juce::var (root.get()), true)))
    {
        error = "Could not write template";
        return false;
    }
    return true;
}

} // namespace autosampler
