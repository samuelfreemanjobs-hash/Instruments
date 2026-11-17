#include "PluginEditor.h"

OvernightDrumAudioProcessorEditor::OvernightDrumAudioProcessorEditor(OvernightDrumAudioProcessor& p)
    : AudioProcessorEditor(p),
      proc_(p),
      keyboard_(p.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setSize(640, 280);
    addAndMakeVisible(keyboard_);
    addAndMakeVisible(reloadBtn_);
    addAndMakeVisible(status_);
    addAndMakeVisible(tune_);
    addAndMakeVisible(decay_);
    addAndMakeVisible(gain_);

    tune_.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    decay_.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gain_.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    tune_.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    decay_.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    gain_.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);

    tuneAtt_ = std::make_unique<Attachment>(proc_.getAPVTS(), "tune", tune_);
    decayAtt_ = std::make_unique<Attachment>(proc_.getAPVTS(), "decay", decay_);
    gainAtt_ = std::make_unique<Attachment>(proc_.getAPVTS(), "gain", gain_);

    status_.setText("Kit: " + proc_.getKitDirectory().getFullPathName(), juce::dontSendNotification);
    reloadBtn_.onClick = [this] {
        const auto dir = juce::File(__FILE__)
                             .getParentDirectory()
                             .getParentDirectory()
                             .getParentDirectory()
                             .getChildFile("output")
                             .getChildFile("best_kit");
        proc_.loadKitFromDirectory(dir);
        status_.setText("Kit: " + dir.getFullPathName(), juce::dontSendNotification);
    };
}

void OvernightDrumAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff121214));
    g.setColour(juce::Colour(0xffc9a227));
    g.setFont(18.0f);
    g.drawText("DiskLordz Overnight Drum", getLocalBounds().removeFromTop(28),
               juce::Justification::centred);
}

void OvernightDrumAudioProcessorEditor::resized()
{
    auto r = getLocalBounds().reduced(8);
    r.removeFromTop(28);
    auto knobs = r.removeFromTop(100);
    tune_.setBounds(knobs.removeFromLeft(100));
    decay_.setBounds(knobs.removeFromLeft(100));
    gain_.setBounds(knobs.removeFromLeft(100));
    reloadBtn_.setBounds(knobs.removeFromLeft(140).reduced(8));
    status_.setBounds(r.removeFromTop(24));
    keyboard_.setBounds(r);
}
