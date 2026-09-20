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

void setVisibleArray (bool on, auto& arr)
{
    for (auto& c : arr)
        c.setVisible (on);
}
} // namespace

SP1200AudioProcessorEditor::SP1200AudioProcessorEditor (SP1200AudioProcessor& p)
    : AudioProcessorEditor (&p), processor_ (p)
{
    setResizeLimits (900, 640, 1600, 1000);
    setSize (1100, 720);

    const int tabGroup = 9001;
    for (auto* tab : { &consoleTab_, &seqTab_, &songTab_ })
    {
        tab->setClickingTogglesState (true);
        tab->setRadioGroupId (tabGroup);
        addAndMakeVisible (*tab);
    }

    seqTab_.setButtonText ("MOD 20 PIANO ROLL");
    consoleTab_.addListener (this);
    seqTab_.addListener (this);
    songTab_.addListener (this);
    consoleTab_.setToggleState (true, juce::dontSendNotification);

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

    chopButton_.onClick = [this] { runAutoChop16(); };
    addAndMakeVisible (chopButton_);

    multiPitchButton_.onClick = [this]
    {
        auto& eng = processor_.engine();
        eng.setMultiPitchEnabled (multiPitchButton_.getToggleState());
        if (multiPitchButton_.getToggleState())
        {
            for (int p = 0; p < sp1200::kNumPads; ++p)
            {
                const int seg = eng.getPad (p).segmentIndex;
                if (seg >= 0)
                {
                    eng.setMultiPitchSourceSegment (seg);
                    break;
                }
            }
        }
    };
    addAndMakeVisible (multiPitchButton_);

    faderModeButton_.onClick = [this]
    {
        faderMode_ = (faderMode_ + 1) % 3;
        processor_.setFaderMode (faderMode_);
        const char* names[] = { "VOL", "PITCH", "DECAY" };
        faderModeButton_.setButtonText ("FADER: " + juce::String (names[faderMode_]));
    };
    addAndMakeVisible (faderModeButton_);

    patternSlider_.setRange (1, sp1200::kMaxPatterns, 1);
    patternSlider_.setValue (1);
    patternSlider_.onValueChange = [this]
    {
        processor_.engine().sequencer().setCurrentPattern (static_cast<int> (patternSlider_.getValue()) - 1);
        syncPianoRollFromControls();
        refreshSeqInfo();
    };
    addAndMakeVisible (patternSlider_);

    barsSlider_.setRange (sp1200::kMinPatternBars, sp1200::kMaxPatternBars, 1);
    barsSlider_.setValue (2);
    barsSlider_.onValueChange = [this]
    {
        processor_.engine().sequencer().setPatternBars (static_cast<int> (barsSlider_.getValue()));
        syncPianoRollFromControls();
        refreshSeqInfo();
    };
    addAndMakeVisible (barsSlider_);

    seqInfoLabel_.setText ("PAT 01 | 2 bars | 1/16", juce::dontSendNotification);
    addAndMakeVisible (seqInfoLabel_);

    seqPlayButton_.onClick = [this] { processor_.engine().sequencer().startPattern(); };
    seqStopButton_.onClick = [this] { processor_.engine().sequencer().stop(); };
    addAndMakeVisible (seqPlayButton_);
    addAndMakeVisible (seqStopButton_);

    seqRecordSteps_.onClick = [this] { seqRecordMode_ = seqRecordSteps_.getToggleState(); };
    addAndMakeVisible (seqRecordSteps_);

    chromaticMapButton_.onClick = [this]
    {
        if (pianoRoll_ != nullptr)
            pianoRoll_->setChromaticMode (chromaticMapButton_.getToggleState());
        chromaticTuneBox_.setVisible (view_ == ViewMode::sequencer && chromaticMapButton_.getToggleState());
        resized();
    };
    addAndMakeVisible (chromaticMapButton_);

    for (int i = 0; i < sp1200::kMultiPitchSlots; ++i)
    {
        const float st = sp1200::kDefaultMultiPitchOffsets[i];
        chromaticTuneBox_.addItem (juce::String (st, 1) + " st", i + 1);
    }
    chromaticTuneBox_.setSelectedId (1);
    chromaticTuneBox_.onChange = [this]
    {
        const int idx = chromaticTuneBox_.getSelectedItemIndex();
        if (idx >= 0 && idx < sp1200::kMultiPitchSlots && pianoRoll_ != nullptr)
            pianoRoll_->setChromaticTune (sp1200::kDefaultMultiPitchOffsets[idx]);
    };
    addAndMakeVisible (chromaticTuneBox_);

    clearPatternButton_.onClick = [this]
    {
        processor_.engine().sequencer().clearCurrentPattern();
        syncPianoRollFromControls();
        refreshSeqInfo();
    };
    addAndMakeVisible (clearPatternButton_);

    pianoRoll_ = std::make_unique<PianoRollComponent> (
        processor_.engine().sequencer(),
        [this] (int padIndex) -> juce::String
        {
            const int seg = processor_.engine().getPad (padIndex).segmentIndex;
            if (seg < 0)
                return "P" + juce::String (padIndex + 1).paddedLeft ('0', 2);
            const auto* s = processor_.engine().memoryPool().getSegment (static_cast<std::size_t> (seg));
            if (s == nullptr || s->name.empty())
                return "P" + juce::String (padIndex + 1).paddedLeft ('0', 2);
            return juce::String (s->name).substring (0, 10);
        });
    addAndMakeVisible (*pianoRoll_);

    songInfoLabel_.setText ("Song chain (8 slots) → patterns 1–99", juce::dontSendNotification);
    addAndMakeVisible (songInfoLabel_);

    songPlayButton_.onClick = [this] { processor_.engine().sequencer().startSong(); };
    addAndMakeVisible (songPlayButton_);

    for (int i = 0; i < static_cast<int> (songSlotBoxes_.size()); ++i)
    {
        auto& box = songSlotBoxes_[static_cast<std::size_t> (i)];
        for (int p = 1; p <= sp1200::kMaxPatterns; ++p)
            box.addItem ("PAT " + juce::String (p).paddedLeft ('0', 2), p);
        box.addItem ("END", -1);
        box.setSelectedId (i + 1);
        box.onChange = [this, i]
        {
            const int id = songSlotBoxes_[static_cast<std::size_t> (i)].getSelectedId();
            processor_.engine().sequencer().setSongSlot (i, id - 1);
        };
        addAndMakeVisible (box);
    }

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
    setView (ViewMode::console);
}

SP1200AudioProcessorEditor::~SP1200AudioProcessorEditor() = default;

void SP1200AudioProcessorEditor::buttonClicked (juce::Button* button)
{
    if (button == &consoleTab_)
        setView (ViewMode::console);
    else if (button == &seqTab_)
        setView (ViewMode::sequencer);
    else if (button == &songTab_)
        setView (ViewMode::song);
}

void SP1200AudioProcessorEditor::setView (ViewMode mode)
{
    view_ = mode;
    const bool console = mode == ViewMode::console;
    const bool seq = mode == ViewMode::sequencer;
    const bool song = mode == ViewMode::song;

    consoleTab_.setToggleState (console, juce::dontSendNotification);
    seqTab_.setToggleState (seq, juce::dontSendNotification);
    songTab_.setToggleState (song, juce::dontSendNotification);

    importButton_.setVisible (console);
    recordButton_.setVisible (console);
    chopButton_.setVisible (console);
    multiPitchButton_.setVisible (console);
    faderModeButton_.setVisible (console || seq);

    patternSlider_.setVisible (seq);
    barsSlider_.setVisible (seq);
    seqInfoLabel_.setVisible (seq);
    seqPlayButton_.setVisible (seq);
    seqStopButton_.setVisible (seq || song);
    seqRecordSteps_.setVisible (seq);
    chromaticMapButton_.setVisible (seq);
    chromaticTuneBox_.setVisible (seq && chromaticMapButton_.getToggleState());
    clearPatternButton_.setVisible (seq);
    if (pianoRoll_ != nullptr)
    {
        pianoRoll_->setVisible (seq);
        if (seq)
            pianoRoll_->toFront (false);
    }

    songInfoLabel_.setVisible (song);
    songPlayButton_.setVisible (song);
    for (auto& box : songSlotBoxes_)
        box.setVisible (song);

    for (auto& b : padButtons_)
        b.setVisible (console);
    for (auto& f : faders_)
        f.setVisible (console);

    if (seq && pianoRoll_ != nullptr)
        syncPianoRollFromControls();

    resized();
    repaint();
}

void SP1200AudioProcessorEditor::syncPianoRollFromControls()
{
    if (pianoRoll_ == nullptr)
        return;
    pianoRoll_->setChromaticMode (chromaticMapButton_.getToggleState());
    const int idx = chromaticTuneBox_.getSelectedItemIndex();
    if (idx >= 0 && idx < sp1200::kMultiPitchSlots)
        pianoRoll_->setChromaticTune (sp1200::kDefaultMultiPitchOffsets[idx]);
    pianoRoll_->refreshFromPattern();
}

void SP1200AudioProcessorEditor::refreshSeqInfo()
{
    auto& seq = processor_.engine().sequencer();
    const int pat = seq.currentPattern() + 1;
    const int bars = seq.patternBars();
    seqInfoLabel_.setText ("PAT " + juce::String (pat).paddedLeft ('0', 2) + " | " + juce::String (bars)
                               + " bar(s) | 1/16 | steps: "
                               + juce::String (seq.pattern (seq.currentPattern()).steps.size()),
                           juce::dontSendNotification);
}

void SP1200AudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xffd9d2c4));
    g.setColour (juce::Colours::black.withAlpha (0.15f));
    g.fillRect (getLocalBounds().removeFromTop (88));
}

void SP1200AudioProcessorEditor::resized()
{
    auto r = getLocalBounds().reduced (8);
    auto tabs = r.removeFromTop (28);
    consoleTab_.setBounds (tabs.removeFromLeft (120).reduced (2));
    seqTab_.setBounds (tabs.removeFromLeft (120).reduced (2));
    songTab_.setBounds (tabs.removeFromLeft (120).reduced (2));
    consoleTab_.toFront (false);
    seqTab_.toFront (false);
    songTab_.toFront (false);

    auto top = r.removeFromTop (40);
    headerLabel_.setBounds (top.removeFromLeft (360));
    engagedLabel_.setBounds (top.removeFromLeft (260));
    memoryLabel_.setBounds (top);

    auto bar = r.removeFromTop (32);
    if (view_ == ViewMode::console)
    {
        importButton_.setBounds (bar.removeFromLeft (120).reduced (2));
        recordButton_.setBounds (bar.removeFromLeft (120).reduced (2));
        chopButton_.setBounds (bar.removeFromLeft (180).reduced (2));
        multiPitchButton_.setBounds (bar.removeFromLeft (160).reduced (2));
        faderModeButton_.setBounds (bar.removeFromLeft (140).reduced (2));
    }
    else if (view_ == ViewMode::sequencer)
    {
        patternSlider_.setBounds (bar.removeFromLeft (160).reduced (2));
        barsSlider_.setBounds (bar.removeFromLeft (120).reduced (2));
        seqPlayButton_.setBounds (bar.removeFromLeft (110).reduced (2));
        seqStopButton_.setBounds (bar.removeFromLeft (70).reduced (2));
        chromaticMapButton_.setBounds (bar.removeFromLeft (130).reduced (2));
        chromaticTuneBox_.setBounds (bar.removeFromLeft (90).reduced (2));
        clearPatternButton_.setBounds (bar.removeFromLeft (100).reduced (2));
        seqRecordSteps_.setBounds (bar.removeFromLeft (140).reduced (2));
        seqInfoLabel_.setBounds (bar);
    }
    else if (view_ == ViewMode::song)
    {
        songPlayButton_.setBounds (bar.removeFromLeft (120).reduced (2));
        seqStopButton_.setBounds (bar.removeFromLeft (80).reduced (2));
        songInfoLabel_.setBounds (bar);
    }
    rateLabel_.setBounds (r.removeFromTop (24));

    if (view_ == ViewMode::song)
    {
        auto songArea = r.reduced (4);
        const int rowH = songArea.getHeight() / 8;
        for (int i = 0; i < 8; ++i)
        {
            auto row = songArea.removeFromTop (rowH);
            songSlotBoxes_[static_cast<std::size_t> (i)].setBounds (row.removeFromLeft (200).reduced (2));
        }
        return;
    }

    if (view_ == ViewMode::sequencer && pianoRoll_ != nullptr)
    {
        pianoRoll_->setBounds (r);
        return;
    }

    if (view_ == ViewMode::console)
    {
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
}

void SP1200AudioProcessorEditor::timerCallback()
{
    refreshMemoryLabel();
    if (view_ == ViewMode::sequencer)
    {
        refreshSeqInfo();
        if (pianoRoll_ != nullptr && processor_.engine().sequencer().isPlaying())
            pianoRoll_->repaint();
    }
}

void SP1200AudioProcessorEditor::refreshMemoryLabel()
{
    const auto& pool = processor_.engine().memoryPool();
    memoryLabel_.setText ("MEMORY: " + formatMemoryTime (pool.usedSamples()) + " / 7:00 TOTAL",
                          juce::dontSendNotification);
}

void SP1200AudioProcessorEditor::triggerPad (int padIndex)
{
    if (view_ == ViewMode::sequencer && seqRecordMode_)
        processor_.engine().recordStepOnCurrentPattern (padIndex, 0.9f);
    else
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
                              lastSegmentForChop_ = static_cast<int> (*idx);
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

void SP1200AudioProcessorEditor::runAutoChop16()
{
    if (lastSegmentForChop_ < 0)
    {
        juce::AlertWindow::showMessageBoxAsync (juce::MessageBoxIconType::InfoIcon,
                                              "Auto-chop",
                                              "Import a sample first.");
        return;
    }
    const auto slices = processor_.engine().autoChopSegment (static_cast<std::size_t> (lastSegmentForChop_), 16);
    if (slices.empty())
    {
        juce::AlertWindow::showMessageBoxAsync (juce::MessageBoxIconType::WarningIcon,
                                              "Auto-chop",
                                              "No transients detected.");
        return;
    }
    for (std::size_t i = 0; i < slices.size() && i < static_cast<std::size_t> (sp1200::kNumPads); ++i)
        processor_.engine().assignSegmentToPad (static_cast<int> (i), static_cast<int> (slices[i]));
    refreshMemoryLabel();
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
            lastSegmentForChop_ = static_cast<int> (*idx);
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
