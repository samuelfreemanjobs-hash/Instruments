#include "PluginEditor.h"

RenaissanceHostAudioProcessorEditor::RenaissanceHostAudioProcessorEditor (RenaissanceHostAudioProcessor& p)
    : juce::AudioProcessorEditor (p),
      processor_ (p)
{
    titleLabel_.setText ("Renaissance Host (JUCE)", juce::dontSendNotification);
    titleLabel_.setFont (juce::FontOptions (18.0f));
    addAndMakeVisible (titleLabel_);

    padLabel_.setText ("Last pad: —", juce::dontSendNotification);
    qlinkLabel_.setText ("Last Q-Link: —", juce::dontSendNotification);
    lcdLabel_.setText ("LCD stub: —", juce::dontSendNotification);
    for (auto* l : { &padLabel_, &qlinkLabel_, &lcdLabel_ })
    {
        l->setFont (juce::FontOptions (14.0f));
        addAndMakeVisible (*l);
    }

    for (int i = 0; i < renaissance::kNumPads; ++i)
    {
        auto& b = padButtons_[static_cast<std::size_t> (i)];
        b.setButtonText (juce::String (i + 1));
        b.setEnabled (false);
        addAndMakeVisible (b);
    }

    setSize (520, 320);
    startTimerHz (30);
}

RenaissanceHostAudioProcessorEditor::~RenaissanceHostAudioProcessorEditor()
{
    stopTimer();
}

void RenaissanceHostAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1a1a1e));
    g.setColour (juce::Colours::white.withAlpha (0.85f));
    g.drawFittedText ("MPC Renaissance surface map: notes 36–51, Q-Link CC 1–16, MIDI omni",
                      getLocalBounds().removeFromBottom (24),
                      juce::Justification::centred,
                      1);
}

void RenaissanceHostAudioProcessorEditor::resized()
{
    auto r = getLocalBounds().reduced (12);
    titleLabel_.setBounds (r.removeFromTop (28));
    padLabel_.setBounds (r.removeFromTop (22));
    qlinkLabel_.setBounds (r.removeFromTop (22));
    lcdLabel_.setBounds (r.removeFromTop (22));
    r.removeFromTop (8);

    auto grid = r;
    const int cols = 8;
    const int rows = 2;
    const int w = grid.getWidth() / cols;
    const int h = grid.getHeight() / rows;
    for (int i = 0; i < renaissance::kNumPads; ++i)
    {
        const int row = i / cols;
        const int col = i % cols;
        padButtons_[static_cast<std::size_t> (i)].setBounds (col * w, grid.getY() + row * h, w - 4, h - 4);
    }
}

void RenaissanceHostAudioProcessorEditor::timerCallback()
{
    int pad = -1;
    float vel = 0.0f;
    int qlinkPad = -1;
    float qlinkVal = 0.0f;
    {
        const juce::ScopedLock lock (processor_.activityLock_);
        pad = processor_.lastPadIndex_;
        vel = processor_.lastVelocity_;
        qlinkPad = processor_.lastQlinkPad_;
        qlinkVal = processor_.lastQlinkValue_;
    }

    for (int i = 0; i < renaissance::kNumPads; ++i)
    {
        const bool lit = i == pad || i == qlinkPad;
        padButtons_[static_cast<std::size_t> (i)].setColour (juce::TextButton::buttonOnColourId,
                                                              lit ? juce::Colours::orange : juce::Colour (0xff33333a));
    }

    padLabel_.setText (pad >= 0 ? juce::String ("Last pad: ") + juce::String (pad + 1) + " vel "
                                      + juce::String (vel, 2)
                                : juce::String ("Last pad: —"),
                       juce::dontSendNotification);
    qlinkLabel_.setText (qlinkPad >= 0 ? juce::String ("Last Q-Link: pad ") + juce::String (qlinkPad + 1) + " = "
                                              + juce::String (qlinkVal, 2)
                                        : juce::String ("Last Q-Link: —"),
                         juce::dontSendNotification);
    lcdLabel_.setText ("LCD stub: " + processor_.lcdBridge().lastLineSent(), juce::dontSendNotification);
}
