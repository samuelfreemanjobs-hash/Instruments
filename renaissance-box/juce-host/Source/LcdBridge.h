#pragma once

#include <JuceHeader.h>

namespace renaissance
{
/** Stub for Renaissance LCD / ring SysEx — fill after MIDI capture on hardware. */
class LcdBridge
{
public:
    void setMidiOutput (juce::MidiOutput* output) noexcept { midiOut_ = output; }

    /** Future: encode Akai SysEx and send to Renaissance display. */
    void sendStatusLine (const juce::String& text);

    [[nodiscard]] juce::String lastLineSent() const noexcept { return lastLine_; }

private:
    juce::MidiOutput* midiOut_ = nullptr;
    juce::String lastLine_;
};

} // namespace renaissance
