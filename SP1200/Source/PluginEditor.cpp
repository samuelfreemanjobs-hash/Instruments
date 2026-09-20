#include "PluginEditor.h"

#include "SP1200Constants.h"

namespace
{
juce::String formatMemoryTime (std::int64_t samples)
{
    const auto seconds = samples / static_cast<std::int64_t> (sp1200::kSampleRateHz);
    const int mm = static_cast<int> (seconds / 60);
    const int ss = static_cast<int> (seconds % 60);
    return juce::String::formatted ("%d:%02d", mm, ss);
}
} // namespace

SP1200AudioProcessorEditor::SP1200AudioProcessorEditor (SP1200AudioProcessor& p)
    : AudioProcessorEditor (&p), processor_ (p)
{
    setResizeLimits (900, 640, 1600, 1000);
    setSize (1100, 720);

    headerLabel_.setText ("SP-1200 SAMPLING DRUMULATOR", juce::dontSendNotification);
    headerLabel_.setFont (juce::FontOptions (18.0f, juce::Font::bold));
    addAndMakeVisible (headerLabel_);

    engagedLabel_.setText ("12-BIT 26.040 kHz ENGAGED", juce::dontSendNotification);
    engagedLabel_.setColour (juce::Label::textColourId, juce::Colour (0xff1a6b32));
    addAndMakeVisible (engagedLabel_);

    rateLabel_.setText ("SAMPLE RATE: 26.040 kHz 12-BIT LINEAR PCM", juce::dontSendNotification);
    addAndMakeVisible (rateLabel_);

    memoryLabel_.setJustificationType (juce::Justification::centredRight);
    addAndMakeVisible (memoryLabel_);

    importButton_.onClick = [this] { importSample(); };
    addAndMakeVisible (importButton_);

    recordButton_.onClick = [this] { toggleRecordInput(); };
    addAndMakeVisible (recordButton_);

    faderModeButton_.onClick = [this]
    {
        faderMode_ = (faderMode_ + 1) % 3;
        processor_.setFaderMode (faderMode_);
        const char* names[] = { "VOL", "PITCH", "DECAY" };
        faderModeButton_.setButtonText ("FADER: " + juce::String (names[faderMode_]));
    };
    addAndMakeVisible (faderModeButton_);

    for (int i = 0; i < sp1200::kNumPads; ++i)
    {
        padButtons_[static_cast<std::size_t> (i)].setButtonText ("PAD " + juce::String (i + 1));
        padButtons_[static_cast<std::size_t> (i)].onClick = [this, i] { triggerPad (i); };
        addAndMakeVisible (padButtons_[static_cast<std::size_t> (i)]);

        auto& s = faders_[static_cast<std::size_t> (i)];
        s.setSliderStyle (juce::Slider::LinearVertical);
        s.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        s.setRange (0.0, 1.0, 0.001);
        s.setValue (0.8);
        s.onValueChange = [this, i]
        {
            if (faderMode_ == 1)
                processor_.engine().setPadTune (i, static_cast<float> (faders_[static_cast<std::size_t> (i)].getValue() * 24.0 - 12.0));
            else if (faderMode_ == 2)
                processor_.engine().setPadLevel (i, static_cast<float> (faders_[static_cast<std::size_t> (i)].getValue()));
            else
                processor_.engine().setPadLevel (i, static_cast<float> (faders_[static_cast<std::size_t> (i)].getValue() * 2.0));
        };
        addAndMakeVisible (s);
    }

    startTimerHz (4);
    refreshMemoryLabel();
}

SP1200AudioProcessorEditor::~SP1200AudioProcessorEditor() = default;

void SP1200AudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xffd9d2c4));
    g.setColour (juce::Colours::black.withAlpha (0.15f));
    g.fillRect (getLocalBounds().removeFromTop (48));
}

void SP1200AudioProcessorEditor::resized()
{
    auto r = getLocalBounds().reduced (8);
    auto top = r.removeFromTop (40);
    headerLabel_.setBounds (top.removeFromLeft (360));
    engagedLabel_.setBounds (top.removeFromLeft (260));
    memoryLabel_.setBounds (top);

    auto bar = r.removeFromTop (32);
    importButton_.setBounds (bar.removeFromLeft (120).reduced (2));
    recordButton_.setBounds (bar.removeFromLeft (120).reduced (2));
    faderModeButton_.setBounds (bar.removeFromLeft (140).reduced (2));
    rateLabel_.setBounds (bar);

    auto padArea = r.removeFromBottom (160);
    auto faderArea = r;

    const int cols = 8;
    const int rows = 2;
    const int cellW = padArea.getWidth() / cols;
    const int fCellH = faderArea.getHeight() / rows;
    const int pCellH = padArea.getHeight() / rows;

    for (int row = 0; row < rows; ++row)
    {
        for (int col = 0; col < cols; ++col)
        {
            const int idx = row * cols + col;
            faders_[static_cast<std::size_t> (idx)].setBounds (col * cellW + 8,
                                                                 row * fCellH + 4,
                                                                 cellW - 16,
                                                                 fCellH - 8);
            padButtons_[static_cast<std::size_t> (idx)].setBounds (col * cellW + 4,
                                                                   padArea.getY() + row * pCellH + 4,
                                                                   cellW - 8,
                                                                   pCellH - 8);
        }
    }
}

void SP1200AudioProcessorEditor::timerCallback()
{
    refreshMemoryLabel();
}

void SP1200AudioProcessorEditor::refreshMemoryLabel()
{
    const auto& pool = processor_.engine().memoryPool();
    const auto used = formatMemoryTime (pool.usedSamples());
    const auto total = juce::String ("7:00");
    memoryLabel_.setText ("MEMORY: " + used + " / " + total + " TOTAL", juce::dontSendNotification);
}

void SP1200AudioProcessorEditor::triggerPad (int padIndex)
{
    processor_.triggerPad (padIndex, 0.9f);
}

void SP1200AudioProcessorEditor::importSample()
{
    auto chooser = std::make_shared<juce::FileChooser> ("Import WAV", juce::File(), "*.wav;*.aif;*.aiff;*.flac");
    chooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                          [this, chooser] (const juce::FileChooser& fc)
                          {
                              const auto f = fc.getResult();
                              if (f == juce::File())
                                  return;
                              auto idx = processor_.engine().importFile (f, 0, f.getFileNameWithoutExtension());
                              if (! idx.has_value())
                              {
                                  juce::AlertWindow::showMessageBoxAsync (juce::MessageBoxIconType::WarningIcon,
                                                                          "Import failed",
                                                                          "Could not import (memory cap or format).");
                                  return;
                              }
                              for (int p = 0; p < sp1200::kNumPads; ++p)
                              {
                                  if (processor_.engine().getPad (p).segmentIndex < 0)
                                  {
                                      processor_.engine().assignSegmentToPad (p, static_cast<int> (*idx));
                                      break;
                                  }
                              }
                              refreshMemoryLabel();
                          });
}

void SP1200AudioProcessorEditor::toggleRecordInput()
{
    auto& eng = processor_.engine();
    if (! eng.isRecording())
    {
        eng.startRecording();
        recordButton_.setButtonText ("STOP + COMMIT");
        recordButton_.setColour (juce::TextButton::buttonColourId, juce::Colours::darkred);
    }
    else
    {
        const auto idx = eng.stopRecordingAndCommit (0);
        if (idx.has_value())
        {
            for (int p = 0; p < sp1200::kNumPads; ++p)
            {
                if (processor_.engine().getPad (p).segmentIndex < 0)
                {
                    processor_.engine().assignSegmentToPad (p, static_cast<int> (*idx));
                    break;
                }
            }
            refreshMemoryLabel();
        }
        recordButton_.setButtonText ("ARM INPUT");
        recordButton_.setColour (juce::TextButton::buttonColourId,
                                 getLookAndFeel().findColour (juce::TextButton::buttonColourId));
    }
}
