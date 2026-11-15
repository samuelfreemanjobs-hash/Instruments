#include "Vst3PluginCache.h"

namespace autosampler
{

Vst3PluginCache::Vst3PluginCache (juce::File cacheFile)
{
    setCacheFile (std::move (cacheFile));
}

void Vst3PluginCache::setCacheFile (const juce::File& file)
{
    cacheFile_ = file;
    loadFromDisk();
}

void Vst3PluginCache::loadFromDisk()
{
    if (! cacheFile_.existsAsFile())
    {
        juce::DynamicObject::Ptr root = new juce::DynamicObject();
        root->setProperty ("format", "autosampler-vst3-cache-v1");
        root->setProperty ("entries", juce::Array<juce::var>());
        state_ = juce::var (root.get());
        return;
    }

    state_ = juce::JSON::parse (cacheFile_);
    if (state_.isVoid())
    {
        juce::DynamicObject::Ptr root = new juce::DynamicObject();
        root->setProperty ("format", "autosampler-vst3-cache-v1");
        root->setProperty ("entries", juce::Array<juce::var>());
        state_ = juce::var (root.get());
    }
}

void Vst3PluginCache::saveToDisk() const
{
    if (! cacheFile_.getFullPathName().isNotEmpty())
        return;

    cacheFile_.getParentDirectory().createDirectory();
    cacheFile_.replaceWithText (juce::JSON::toString (state_, true));
}

static juce::String pathKey (const juce::File& f)
{
    return f.getFullPathName();
}

bool Vst3PluginCache::wasLastLoadOk (const juce::File& vst3Path) const
{
    if (auto* arr = state_.getProperty ("entries", {}).getArray())
    {
        const auto key = pathKey (vst3Path);
        for (const auto& e : *arr)
        {
            if (e.getProperty ("path", {}).toString() == key)
                return static_cast<bool> (e.getProperty ("lastOk", false));
        }
    }
    return false;
}

void Vst3PluginCache::markOk (const juce::File& vst3Path)
{
    auto* arr = state_.getProperty ("entries", {}).getArray();
    if (arr == nullptr)
    {
        state_.getDynamicObject()->setProperty ("entries", juce::Array<juce::var>());
        arr = state_.getProperty ("entries", {}).getArray();
    }

    const auto key = pathKey (vst3Path);
    for (int i = 0; i < arr->size(); ++i)
    {
        if ((*arr)[i].getProperty ("path", {}).toString() == key)
        {
            if (auto* obj = (*arr)[i].getDynamicObject())
            {
                obj->setProperty ("lastOk", true);
                obj->setProperty ("updatedAtMs", juce::Time::getCurrentTime().toMilliseconds());
            }
            saveToDisk();
            return;
        }
    }

    juce::DynamicObject::Ptr entry = new juce::DynamicObject();
    entry->setProperty ("path", key);
    entry->setProperty ("lastOk", true);
    entry->setProperty ("updatedAtMs", juce::Time::getCurrentTime().toMilliseconds());
    arr->add (juce::var (entry.get()));
    saveToDisk();
}

void Vst3PluginCache::markFailed (const juce::File& vst3Path)
{
    auto* arr = state_.getProperty ("entries", {}).getArray();
    if (arr == nullptr)
        return;

    const auto key = pathKey (vst3Path);
    for (int i = 0; i < arr->size(); ++i)
    {
        if ((*arr)[i].getProperty ("path", {}).toString() == key)
        {
            if (auto* obj = (*arr)[i].getDynamicObject())
            {
                obj->setProperty ("lastOk", false);
                obj->setProperty ("updatedAtMs", juce::Time::getCurrentTime().toMilliseconds());
            }
            saveToDisk();
            return;
        }
    }

    juce::DynamicObject::Ptr entry = new juce::DynamicObject();
    entry->setProperty ("path", key);
    entry->setProperty ("lastOk", false);
    entry->setProperty ("updatedAtMs", juce::Time::getCurrentTime().toMilliseconds());
    arr->add (juce::var (entry.get()));
    saveToDisk();
}

} // namespace autosampler
