#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace dl_ui
{

/** Horizontal keyboard (3+ octaves) with octave shift; feeds a shared MidiKeyboardState. */
class OnScreenKeyboardStrip final : public juce::Component
{
public:
    OnScreenKeyboardStrip (juce::MidiKeyboardState& state, int midiChannel = 1);

    void setOctaves (int count);
    int getLowestVisibleMidiNote() const noexcept { return lowestNote_; }

    void resized() override;

private:
    void shiftOctave (int delta);
    void rebuildKeyboard();

    juce::MidiKeyboardState& state_;
    const int midiChannel_;
    int octaveShift_ = 0;
    int octaveCount_ = 3;

    juce::TextButton downOctave_ { "<" };
    juce::TextButton upOctave_ { ">" };
    juce::Label octaveLabel_;
    std::unique_ptr<juce::MidiKeyboardComponent> keyboard_;
    int lowestNote_ = 48;
};

} // namespace dl_ui