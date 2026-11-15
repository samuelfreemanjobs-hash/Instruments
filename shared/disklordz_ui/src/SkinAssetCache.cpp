#include "dl_ui/SkinAssetCache.h"

namespace dl_ui
{

void SkinAssetCache::registerBinary (const juce::String& name, const void* data, int size)
{
    if (data == nullptr || size <= 0)
        return;
    cache_[name] = juce::ImageFileFormat::loadFrom (data, static_cast<std::size_t> (size));
}

juce::Image SkinAssetCache::getImage (const juce::String& resourceName)
{
    const auto it = cache_.find (resourceName);
    if (it != cache_.end())
        return it->second;
    return {};
}

juce::Image SkinAssetCache::getFilmstripFrame (const juce::String& resourceName, int frameIndex, int numFrames)
{
    const auto sheet = getImage (resourceName);
    if (! sheet.isValid() || numFrames <= 0)
        return {};

    const int frameH = sheet.getHeight() / numFrames;
    const int y = juce::jlimit (0, numFrames - 1, frameIndex) * frameH;
    return sheet.getClippedImage ({ 0, y, sheet.getWidth(), frameH });
}

} // namespace dl_ui