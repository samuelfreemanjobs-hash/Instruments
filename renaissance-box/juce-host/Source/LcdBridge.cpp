#include "LcdBridge.h"

namespace renaissance
{
void LcdBridge::sendStatusLine (const juce::String& text)
{
    lastLine_ = text;
    if (midiOut_ == nullptr)
        return;

    // TODO: Akai Renaissance LCD SysEx (capture from MPC Software traffic).
    juce::ignoreUnused (midiOut_);
}

} // namespace renaissance
