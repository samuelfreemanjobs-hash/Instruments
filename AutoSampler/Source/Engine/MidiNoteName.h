#pragma once

#include <JuceHeader.h>

namespace autosampler
{

inline juce::String midiNoteName (int note) noexcept
{
    return juce::MidiMessage::getMidiNoteName (juce::jlimit (0, 127, note), true, true, 3);
}

} // namespace autosampler
