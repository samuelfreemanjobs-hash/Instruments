#include "PluginEditor.h"

DrumDesignLabAudioProcessorEditor::DrumDesignLabAudioProcessorEditor(DrumDesignLabAudioProcessor& p)
    : AudioProcessorEditor(&p), proc_(p)
{
    setSize(720, 480);
    title_.setText("DiskLordz Drum Design Lab", juce::dontSendNotification);
    title_.setFont(juce::FontOptions(22.0f, juce::Font::bold));
    title_.setColour(juce::Label::textColourId, juce::Colours::orange);
    addAndMakeVisible(title_);

    styleLabel_.setText("Kit Style", juce::dontSendNotification);
    addAndMakeVisible(styleLabel_);
    for (int i = 0; i < static_cast<int>(disklordz::KitStyle::Count); ++i)
        styleBox_.addItem(disklordz::kitStyleName(static_cast<disklordz::KitStyle>(i)), i + 1);
    styleBox_.setSelectedItemIndex(static_cast<int>(proc_.getKitStyle()), juce::dontSendNotification);
    addAndMakeVisible(styleBox_);
    styleAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        proc_.apvts, "ddl_kit_style", styleBox_);

    gainLabel_.setText("Output", juce::dontSendNotification);
    addAndMakeVisible(gainLabel_);
    gainSlider_.setSliderStyle(juce::Slider::LinearHorizontal);
    gainSlider_.setTextBoxStyle(juce::Slider::TextBoxRight, false, 56, 20);
    addAndMakeVisible(gainSlider_);
    gainAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        proc_.apvts, "ddl_output_gain", gainSlider_);

    peakLabel_.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(peakLabel_);

    buildPadButtons();
    startTimerHz(30);
}

void DrumDesignLabAudioProcessorEditor::buildPadButtons()
{
    static constexpr const char* names[16] = {
        "Kick", "Rim", "Snare", "Clap", "808", "Perc", "CH", "808B",
        "CH2", "Tom", "OH", "FX", "Kick2", "Ride", "Snare2", "Impact"
    };
    for (int i = 0; i < 16; ++i)
    {
        pads_[static_cast<size_t>(i)].setButtonText(names[i]);
        pads_[static_cast<size_t>(i)].onClick = [this, i] {
            // Soft UI trigger into processor MIDI path via note-on simulation
            juce::MidiBuffer mb;
            mb.addEvent(juce::MidiMessage::noteOn(1, 36 + i, (juce::uint8)100), 0);
            juce::AudioBuffer<float> silent(2, 64);
            silent.clear();
            // Direct note into engine through processBlock-compatible call
            proc_.processBlock(silent, mb);
        };
        addAndMakeVisible(pads_[static_cast<size_t>(i)]);
    }
}

void DrumDesignLabAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff121214));
    g.setColour(juce::Colour(0xff2a2a30));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(8.0f), 10.0f);
}

void DrumDesignLabAudioProcessorEditor::resized()
{
    auto r = getLocalBounds().reduced(16);
    title_.setBounds(r.removeFromTop(32));
    r.removeFromTop(8);
    auto row = r.removeFromTop(28);
    styleLabel_.setBounds(row.removeFromLeft(70));
    styleBox_.setBounds(row.removeFromLeft(280));
    row.removeFromLeft(12);
    gainLabel_.setBounds(row.removeFromLeft(50));
    gainSlider_.setBounds(row);
    r.removeFromTop(8);
    peakLabel_.setBounds(r.removeFromTop(20));
    r.removeFromTop(8);

    const int cols = 4;
    const int rows = 4;
    const int padW = r.getWidth() / cols;
    const int padH = r.getHeight() / rows;
    for (int i = 0; i < 16; ++i)
    {
        const int c = i % cols;
        const int rw = i / cols;
        pads_[static_cast<size_t>(i)].setBounds(r.getX() + c * padW + 2, r.getY() + rw * padH + 2,
                                                padW - 4, padH - 4);
    }
}

void DrumDesignLabAudioProcessorEditor::timerCallback()
{
    peakLabel_.setText("Style: " + juce::String(disklordz::kitStyleName(proc_.getKitStyle())),
                       juce::dontSendNotification);
}
