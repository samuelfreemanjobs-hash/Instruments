#include "MegaBankTemplate.h"
#include "AutoRecordEngine.h"

namespace autosampler
{

namespace
{

static juce::String noteMapToString (NoteMapPreset p)
{
    switch (p)
    {
        case NoteMapPreset::minorThird: return "minorThird";
        case NoteMapPreset::majorThird: return "majorThird";
        case NoteMapPreset::perfectFifth: return "fifth";
        case NoteMapPreset::octave: return "octave";
        default: return "chromatic";
    }
}

static juce::String blackKeyToString (BlackKeyPolicy p)
{
    switch (p)
    {
        case BlackKeyPolicy::skip: return "skip";
        case BlackKeyPolicy::whiteKeysOnly: return "whiteKeysOnly";
        default: return "include";
    }
}

juce::var captureConfigToVar (const AutoRecordEngine::Config& cfg)
{
    juce::DynamicObject::Ptr c = new juce::DynamicObject();
    c->setProperty ("lowNote", cfg.lowNote);
    c->setProperty ("highNote", cfg.highNote);
    c->setProperty ("noteStep", cfg.noteStep);
    c->setProperty ("noteMapPreset", noteMapToString (cfg.noteMapPreset));
    c->setProperty ("blackKeyPolicy", blackKeyToString (cfg.blackKeyPolicy));
    c->setProperty ("recordUntilSilence", cfg.recordUntilSilence);
    c->setProperty ("maxRecordSeconds", cfg.maxRecordSeconds);
    c->setProperty ("noteHoldMs", cfg.noteHoldMs);
    c->setProperty ("tailMs", cfg.tailMs);
    c->setProperty ("prerollMs", cfg.prerollMs);
    c->setProperty ("pauseMs", cfg.pauseMs);
    c->setProperty ("midiChannel", cfg.midiChannel);
    c->setProperty ("velocity1", cfg.velocitySlots[0]);
    c->setProperty ("velocity2", cfg.velocitySlots[1]);
    c->setProperty ("velocity3", cfg.velocitySlots[2]);
    c->setProperty ("velocity4", cfg.velocitySlots[3]);
    c->setProperty ("trimSilence", cfg.postProcess.trimSilence);
    c->setProperty ("normalize", cfg.postProcess.normalize);
    c->setProperty ("sustainLoop", cfg.postProcess.findSustainLoop);
    c->setProperty ("trimThresholdDb", cfg.postProcess.trimThresholdDb);
    c->setProperty ("normalizePeakDb", cfg.postProcess.normalizePeakDb);
    c->setProperty ("minLoopConfidence", cfg.postProcess.minLoopConfidence);
    c->setProperty ("silenceThresholdDb", cfg.silenceThresholdDb);
    c->setProperty ("silenceHoldSeconds", cfg.silenceHoldSeconds);
    return juce::var (c.get());
}

} // namespace

bool writeMegaBankJsonTemplate (const juce::File& jsonFile, juce::String& error)
{
    return writeMegaBankJsonTemplate (jsonFile, error, {});
}

bool writeMegaBankJsonTemplate (const juce::File& jsonFile, juce::String& error, const AutoRecordEngine::Config& captureFromPlugin)
{
    juce::DynamicObject::Ptr root = new juce::DynamicObject();
    root->setProperty ("format", "instruments-autosampler-mega-bank-v2");
    root->setProperty ("name", "OvernightMegaBank");

    juce::var capture;
    if (captureFromPlugin.highNote >= captureFromPlugin.lowNote)
        capture = captureConfigToVar (captureFromPlugin);
    else
        capture = juce::JSON::parse (R"({
          "lowNote": 36, "highNote": 96, "noteStep": 3,
          "noteHoldMs": 1000, "tailMs": 2500, "pauseMs": 500,
          "velocity1": 127, "velocity2": 0, "velocity3": 0, "velocity4": 0
        })");

    root->setProperty ("capture", capture);
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
