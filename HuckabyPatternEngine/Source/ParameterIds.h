#pragma once

#include <JuceHeader.h>

namespace huckaby::pid
{

inline juce::String trackField(int trackIndex, const char* suffix)
{
    return "trk" + juce::String(trackIndex) + "_" + suffix;
}

} // namespace huckaby::pid
