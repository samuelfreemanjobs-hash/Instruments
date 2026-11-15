#include "dl_ui/OnScreenKeyboardStrip.h"

namespace dl_ui
{

OnScreenKeyboardStrip::OnScreenKeyboardStrip (juce::MidiKeyboardState& state, int midiChannel)
    : state_ (state), midiChannel_ (midiChannel)
{
    octaveLabel_.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (octaveLabel_);

    downOctave_.onClick = [this] { shiftOctave (-1); };
    upOctave_.onClick = [this] { shiftOctave (1); };
    addAndMakeVisible (downOctave_);
    addAndMakeVisible (upOctave_);

    rebuildKeyboard();
}

void OnScreenKeyboardStrip::setOctaves (int count)
{
    octaveCount_ = juce::jlimit (2, 6, count);
    rebuildKeyboard();
    resized();
}

void OnScreenKeyboardStrip::rebuildKeyboard()
{
    keyboard_.reset();
    lowestNote_ = 48 + octaveShift_ * 12;
    const int numKeys = octaveCount_ * 12 + 1;
    keyboard_ = std::make_unique<juce::MidiKeyboardComponent> (
        state_, juce::MidiKeyboardComponent::horizontalKeyboard);
    keyboard_->setMidiChannel (midiChannel_);
    keyboard_->setKeyWidth (14.0f);
    keyboard_->setLowestVisibleKey (lowestNote_);
    keyboard_->setAvailableRange (lowestNote_, lowestNote_ + numKeys);
    addAndMakeVisible (keyboard_.get());
    octaveLabel_.setText ("Oct " + juce::String (lowestNote_ / 12 - 1), juce::dontSendNotification);
}

void OnScreenKeyboardStrip::shiftOctave (int delta)
{
    octaveShift_ = juce::jlimit (-3, 4, octaveShift_ + delta);
    rebuildKeyboard();
}

void OnScreenKeyboardStrip::resized()
{
    auto r = getLocalBounds().reduced (2);
  auto side = r.removeFromLeft (88);
    downOctave_.setBounds (side.removeFromTop (22).removeFromLeft (32).reduced (1));
    upOctave_.setBounds (side.removeFromTop (22).removeFromLeft (32).reduced (1));
    octaveLabel_.setBounds (side.removeFromTop (22));
    if (keyboard_ != nullptr)
        keyboard_->setBounds (r);
}

} // namespace dl_ui