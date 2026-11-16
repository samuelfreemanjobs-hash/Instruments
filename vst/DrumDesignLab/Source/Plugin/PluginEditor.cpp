#include "PluginEditor.h"

namespace
{
struct KnobSpec
{
    const char* id;
    const char* label;
};

constexpr KnobSpec kKnobSpecs[15] = {
    { "ddl_sub_freq_hz", "Sub Hz" },
    { "ddl_punch_freq_hz", "Punch" },
    { "ddl_pitch_sweep_ms", "Sweep" },
    { "ddl_amp_decay_ms", "Decay" },
    { "ddl_body_level", "Body" },
    { "ddl_attack_level", "Attack" },
    { "ddl_texture_level", "Texture" },
    { "ddl_drive", "Drive" },
    { "ddl_dirt_blend", "Dirt" },
    { "ddl_sampler_grit", "Grit" },
    { "ddl_stereo_width", "Width" },
    { "ddl_reverb_mix", "Space" },
    { "ddl_filter_cutoff_hz", "Cutoff" },
    { "ddl_fm_ratio", "FM Ratio" },
    { "ddl_fm_index", "FM Index" },
};
} // namespace

DrumDesignLabAudioProcessorEditor::DrumDesignLabAudioProcessorEditor(DrumDesignLabAudioProcessor& p)
    : AudioProcessorEditor(&p), proc_(p)
{
    setSize(980, 620);
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

    statusLabel_.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(statusLabel_);

    editTitle_.setText("Sound Edit — select a pad", juce::dontSendNotification);
    editTitle_.setFont(juce::FontOptions(16.0f, juce::Font::bold));
    editTitle_.setColour(juce::Label::textColourId, juce::Colour(0xfff4b35b));
    addAndMakeVisible(editTitle_);

    buildPadButtons();
    buildEditControls();
    refreshPadHighlights();
    startTimerHz(20);
}

void DrumDesignLabAudioProcessorEditor::buildEditControls()
{
    for (size_t i = 0; i < knobs_.size(); ++i)
    {
        auto& k = knobs_[i];
        k.slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        k.slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 16);
        k.slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xfff4b35b));
        k.slider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
        k.label.setText(kKnobSpecs[i].label, juce::dontSendNotification);
        k.label.setJustificationType(juce::Justification::centred);
        k.label.setColour(juce::Label::textColourId, juce::Colour(0xffc8c8d0));
        addAndMakeVisible(k.slider);
        addAndMakeVisible(k.label);
        k.attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            proc_.apvts, kKnobSpecs[i].id, k.slider);
    }
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
        pads_[static_cast<size_t>(i)].setClickingTogglesState(false);
        pads_[static_cast<size_t>(i)].onClick = [this, i] {
            proc_.selectPad(i, true);
            refreshPadHighlights();
        };
        addAndMakeVisible(pads_[static_cast<size_t>(i)]);
    }
}

void DrumDesignLabAudioProcessorEditor::refreshPadHighlights()
{
    const int sel = proc_.getSelectedPad();
    for (int i = 0; i < 16; ++i)
    {
        auto& b = pads_[static_cast<size_t>(i)];
        const bool on = (i == sel);
        b.setColour(juce::TextButton::buttonColourId,
                    on ? juce::Colour(0xfff4b35b) : juce::Colour(0xff2a2a32));
        b.setColour(juce::TextButton::textColourOffId,
                    on ? juce::Colours::black : juce::Colours::white);
    }
}

void DrumDesignLabAudioProcessorEditor::pullKnobsIfNeeded()
{
    if (proc_.consumeKnobsNeedPull())
        proc_.pullKnobsFromEngine();

    const int style = static_cast<int>(proc_.getKitStyle());
    const int pad = proc_.getSelectedPad();
    if (style == lastStyle_ && pad == lastPad_)
        return;
    lastStyle_ = style;
    lastPad_ = pad;
    static constexpr const char* names[16] = {
        "Kick", "Rim", "Snare", "Clap", "808", "Perc", "CH", "808B",
        "CH2", "Tom", "OH", "FX", "Kick2", "Ride", "Snare2", "Impact"
    };
    const char* mode = proc_.engineUsesThreeLayerPads() ? "pad recipe" : "engine voice family";
    editTitle_.setText(juce::String("Sound Edit — ") + names[pad] + " (" + mode + ")",
                       juce::dontSendNotification);
    refreshPadHighlights();
}

void DrumDesignLabAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff121214));
    g.setColour(juce::Colour(0xff1c1c22));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(8.0f), 10.0f);
    g.setColour(juce::Colour(0xff2a2a30));
    auto editArea = getLocalBounds().reduced(16);
    editArea.removeFromTop(100);
    editArea.removeFromLeft(editArea.getWidth() / 2);
    g.fillRoundedRectangle(editArea.toFloat().expanded(4.0f), 8.0f);
}

void DrumDesignLabAudioProcessorEditor::layoutKnob(juce::Slider& s, juce::Label& l, juce::Rectangle<int> area)
{
    l.setBounds(area.removeFromTop(18));
    s.setBounds(area.reduced(4));
}

void DrumDesignLabAudioProcessorEditor::resized()
{
    auto r = getLocalBounds().reduced(16);
    title_.setBounds(r.removeFromTop(32));
    r.removeFromTop(6);
    auto row = r.removeFromTop(28);
    styleLabel_.setBounds(row.removeFromLeft(70));
    styleBox_.setBounds(row.removeFromLeft(300));
    row.removeFromLeft(12);
    gainLabel_.setBounds(row.removeFromLeft(50));
    gainSlider_.setBounds(row.removeFromLeft(220));
    r.removeFromTop(6);
    statusLabel_.setBounds(r.removeFromTop(20));
    r.removeFromTop(8);

    auto left = r.removeFromLeft(r.getWidth() / 2).reduced(4);
    auto right = r.reduced(4);

    editTitle_.setBounds(right.removeFromTop(24));
    right.removeFromTop(6);

    const int cols = 4;
    const int rows = 4;
    const int padW = left.getWidth() / cols;
    const int padH = left.getHeight() / rows;
    for (int i = 0; i < 16; ++i)
    {
        const int c = i % cols;
        const int rw = i / cols;
        pads_[static_cast<size_t>(i)].setBounds(left.getX() + c * padW + 2,
                                                left.getY() + rw * padH + 2,
                                                padW - 4, padH - 4);
    }

    // 15 knobs in 5x3 grid
    const int kCols = 5;
    const int kRows = 3;
    const int kW = right.getWidth() / kCols;
    const int kH = right.getHeight() / kRows;
    for (size_t i = 0; i < knobs_.size(); ++i)
    {
        const int c = static_cast<int>(i) % kCols;
        const int rw = static_cast<int>(i) / kCols;
        auto cell = juce::Rectangle<int>(right.getX() + c * kW, right.getY() + rw * kH, kW, kH).reduced(4);
        layoutKnob(knobs_[i].slider, knobs_[i].label, cell);
    }
}

void DrumDesignLabAudioProcessorEditor::timerCallback()
{
    pullKnobsIfNeeded();
    const auto* name = disklordz::kitStyleName(proc_.getKitStyle());
    statusLabel_.setText(juce::String("Style: ") + name + "  |  Pad " + juce::String(proc_.getSelectedPad() + 1)
                             + "  |  Twist knobs to sculpt the selected sound",
                         juce::dontSendNotification);
}
