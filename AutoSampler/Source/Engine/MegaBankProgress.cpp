#include "MegaBankProgress.h"

namespace autosampler
{

juce::File MegaBankProgress::progressFileFor (const juce::File& outputRoot, const juce::String& bankName)
{
    return outputRoot.getChildFile (bankName.replaceCharacter ('/', '_')).getChildFile ("mega_bank_progress.json");
}

juce::File MegaBankProgress::validationReportFor (const juce::File& outputRoot, const juce::String& bankName)
{
    return outputRoot.getChildFile (bankName.replaceCharacter ('/', '_')).getChildFile ("mpc_validation.txt");
}

juce::File MegaBankProgress::skippedReportFor (const juce::File& outputRoot, const juce::String& bankName)
{
    return outputRoot.getChildFile (bankName.replaceCharacter ('/', '_')).getChildFile ("mega_bank_skipped.txt");
}

bool MegaBankProgress::load (const juce::File& file, juce::var& state, juce::String& error)
{
    if (! file.existsAsFile())
    {
        state = juce::var();
        return true;
    }

    state = juce::JSON::parse (file);
    if (state.isVoid())
    {
        error = "Invalid progress JSON";
        return false;
    }
    return true;
}

bool MegaBankProgress::save (const juce::File& file, const juce::var& state, juce::String& error)
{
    file.getParentDirectory().createDirectory();
    const juce::String json = juce::JSON::toString (state, true);
    if (! file.replaceWithText (json))
    {
        error = "Could not write progress file";
        return false;
    }
    return true;
}

juce::var MegaBankProgress::createEmpty (const juce::String& bankName, int totalSources)
{
    juce::DynamicObject::Ptr root = new juce::DynamicObject();
    root->setProperty ("format", kFormat);
    root->setProperty ("bankName", bankName);
    root->setProperty ("status", "pending");
    root->setProperty ("totalSources", totalSources);
    root->setProperty ("currentSource", {});
    root->setProperty ("completedSources", juce::Array<juce::var>());
    root->setProperty ("failedSources", juce::Array<juce::var>());
    root->setProperty ("updatedAtMs", juce::Time::getCurrentTime().toMilliseconds());
    return juce::var (root.get());
}

bool MegaBankProgress::isSourceCompleted (const juce::var& state, const juce::String& presetName)
{
    if (auto* arr = state.getProperty ("completedSources", {}).getArray())
    {
        for (const auto& e : *arr)
        {
            if (e.getProperty ("presetName", {}).toString() == presetName)
                return true;
        }
    }
    return false;
}

static void touchUpdated (juce::var& state)
{
    if (auto* obj = state.getDynamicObject())
        obj->setProperty ("updatedAtMs", juce::Time::getCurrentTime().toMilliseconds());
}

void MegaBankProgress::markSourceCompleted (juce::var& state, const juce::String& presetName, const juce::File& exportRel)
{
    auto* arr = state.getProperty ("completedSources", {}).getArray();
    if (arr == nullptr)
    {
        state.getDynamicObject()->setProperty ("completedSources", juce::Array<juce::var>());
        arr = state.getProperty ("completedSources", {}).getArray();
    }

    juce::DynamicObject::Ptr entry = new juce::DynamicObject();
    entry->setProperty ("presetName", presetName);
    entry->setProperty ("path", exportRel.getFullPathName());
    entry->setProperty ("completedAtMs", juce::Time::getCurrentTime().toMilliseconds());
    arr->add (juce::var (entry.get()));
    touchUpdated (state);
}

void MegaBankProgress::markSourceFailed (juce::var& state, const juce::String& presetName, const juce::String& err, int attempts)
{
    auto* arr = state.getProperty ("failedSources", {}).getArray();
    if (arr == nullptr)
    {
        state.getDynamicObject()->setProperty ("failedSources", juce::Array<juce::var>());
        arr = state.getProperty ("failedSources", {}).getArray();
    }

    juce::DynamicObject::Ptr entry = new juce::DynamicObject();
    entry->setProperty ("presetName", presetName);
    entry->setProperty ("error", err);
    entry->setProperty ("attempts", attempts);
    arr->add (juce::var (entry.get()));
    touchUpdated (state);
}

void MegaBankProgress::setRunning (juce::var& state, const juce::String& bankName, int totalSources, const juce::String& current)
{
    if (auto* obj = state.getDynamicObject())
    {
        obj->setProperty ("format", kFormat);
        obj->setProperty ("bankName", bankName);
        obj->setProperty ("status", "running");
        obj->setProperty ("totalSources", totalSources);
        obj->setProperty ("currentSource", current);
    }
    touchUpdated (state);
}

void MegaBankProgress::setFinished (juce::var& state, bool success, bool hadSkips)
{
    if (auto* obj = state.getDynamicObject())
    {
        if (! success)
            obj->setProperty ("status", "failed");
        else if (hadSkips)
            obj->setProperty ("status", "completed_with_skips");
        else
            obj->setProperty ("status", "completed");
    }
    touchUpdated (state);
}

} // namespace autosampler
