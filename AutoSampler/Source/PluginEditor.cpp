#include "PluginEditor.h"

namespace
{
constexpr int kW = 780;
constexpr int kH = 720;

juce::String stateToString (autosampler::AutoSampleState st)
{
    switch (st)
    {
        case autosampler::AutoSampleState::idle: return "Idle";
        case autosampler::AutoSampleState::waitingGap: return "Waiting...";
        case autosampler::AutoSampleState::recordingNote: return "Recording";
        case autosampler::AutoSampleState::finished: return "Finished";
        case autosampler::AutoSampleState::aborted: return "Aborted";
    }
    return {};
}
} // namespace

void AutoSamplerAudioProcessorEditor::styleSlider (juce::Slider& s)
{
    s.setSliderStyle (juce::Slider::LinearHorizontal);
    s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 72, 18);
}

AutoSamplerAudioProcessorEditor::AutoSamplerAudioProcessorEditor (AutoSamplerAudioProcessor& p)
    : AudioProcessorEditor (&p),
      processor_ (p),
      progressBar_ (progressValue_)
{
    progressValue_ = 0.0;
    setSize (kW, kH);

    titleLabel_.setFont (juce::Font (24.0f, juce::Font::bold));
    addAndMakeVisible (titleLabel_);
    addAndMakeVisible (statusLabel_);
    addAndMakeVisible (planLabel_);
    addAndMakeVisible (inputMeterLabel_);
    addAndMakeVisible (progressBar_);

    keyboard_.setAvailableRange (0, 127);
    keyboard_.setScrollButtonsVisible (false);
    addAndMakeVisible (keyboard_);

    for (int i = 0; i < p.getNumPrograms(); ++i)
        programBox_.addItem (p.getProgramName (i), i + 1);
    programBox_.setSelectedId (p.getCurrentProgram() + 1, juce::dontSendNotification);
    programBox_.onChange = [this]
    {
        processor_.setCurrentProgram (programBox_.getSelectedId() - 1);
    };
    addAndMakeVisible (programBox_);

    lowNoteSlider_.setRange (0, 127, 1);
    highNoteSlider_.setRange (0, 127, 1);
    noteStepSlider_.setRange (1, 12, 1);
    gateMsSlider_.setRange (50, 10000, 1);
    tailMsSlider_.setRange (50, 30000, 1);
    prerollMsSlider_.setRange (0, 5000, 1);
    pauseMsSlider_.setRange (0, 5000, 1);
    for (auto* s : { &velocity1Slider_, &velocity2Slider_, &velocity3Slider_, &velocity4Slider_ })
        s->setRange (0, 127, 1);
    midiChannelSlider_.setRange (1, 16, 1);
    outputGainSlider_.setRange (-24.0, 12.0, 0.01);

    for (auto* s : { &lowNoteSlider_, &highNoteSlider_, &noteStepSlider_, &gateMsSlider_, &tailMsSlider_,
                     &prerollMsSlider_, &pauseMsSlider_, &velocity1Slider_, &velocity2Slider_, &velocity3Slider_,
                     &velocity4Slider_, &midiChannelSlider_, &outputGainSlider_ })
    {
        styleSlider (*s);
        addAndMakeVisible (*s);
    }

    using PID = autosampler::ParameterIDs;
    lowNoteAttachment_ = std::make_unique<SliderAttachment> (processor_.getApvts(), PID::lowNote, lowNoteSlider_);
    highNoteAttachment_ = std::make_unique<SliderAttachment> (processor_.getApvts(), PID::highNote, highNoteSlider_);
    noteStepAttachment_ = std::make_unique<SliderAttachment> (processor_.getApvts(), PID::noteStep, noteStepSlider_);
    gateMsAttachment_ = std::make_unique<SliderAttachment> (processor_.getApvts(), PID::noteHoldMs, gateMsSlider_);
    tailMsAttachment_ = std::make_unique<SliderAttachment> (processor_.getApvts(), PID::tailMs, tailMsSlider_);
    prerollMsAttachment_ = std::make_unique<SliderAttachment> (processor_.getApvts(), PID::prerollMs, prerollMsSlider_);
    pauseMsAttachment_ = std::make_unique<SliderAttachment> (processor_.getApvts(), PID::gapMs, pauseMsSlider_);
    velocity1Attachment_ = std::make_unique<SliderAttachment> (processor_.getApvts(), PID::velocity1, velocity1Slider_);
    velocity2Attachment_ = std::make_unique<SliderAttachment> (processor_.getApvts(), PID::velocity2, velocity2Slider_);
    velocity3Attachment_ = std::make_unique<SliderAttachment> (processor_.getApvts(), PID::velocity3, velocity3Slider_);
    velocity4Attachment_ = std::make_unique<SliderAttachment> (processor_.getApvts(), PID::velocity4, velocity4Slider_);
    midiChannelAttachment_ = std::make_unique<SliderAttachment> (processor_.getApvts(), PID::midiChannel, midiChannelSlider_);
    outputGainAttachment_ = std::make_unique<SliderAttachment> (processor_.getApvts(), PID::outputGain, outputGainSlider_);
    startAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), PID::startAutoSample, startButton_);
    trimAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), PID::trimSilence, trimButton_);
    normalizeAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), PID::normalizeCapture, normalizeButton_);
    loopAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), PID::sustainLoop, loopButton_);
    untilSilenceAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), PID::recordUntilSilence, untilSilenceButton_);

    gateMsSlider_.setTextValueSuffix (" ms");
    tailMsSlider_.setTextValueSuffix (" ms");
    prerollMsSlider_.setTextValueSuffix (" ms");
    pauseMsSlider_.setTextValueSuffix (" ms");

    for (auto* b : { &startButton_, &exportPresetButton_, &exportMpcKitButton_, &exportBankButton_,
                     &chooseFolderButton_, &reprocessButton_, &megaBankJsonButton_ })
        addAndMakeVisible (*b);
    for (auto* b : { &trimButton_, &normalizeButton_, &loopButton_, &untilSilenceButton_ })
        addAndMakeVisible (*b);

    exportPresetButton_.onClick = [this]
    {
        if (auto* param = processor_.getApvts().getParameter (autosampler::ParameterIDs::exportPreset))
        {
            param->beginChangeGesture();
            param->setValueNotifyingHost (1.0f);
            param->setValueNotifyingHost (0.0f);
            param->endChangeGesture();
        }
    };

    exportMpcKitButton_.onClick = [this]
    {
        if (auto* param = processor_.getApvts().getParameter (autosampler::ParameterIDs::exportMpcKit))
        {
            param->beginChangeGesture();
            param->setValueNotifyingHost (1.0f);
            param->setValueNotifyingHost (0.0f);
            param->endChangeGesture();
        }
    };

    exportBankButton_.onClick = [this]
    {
        const auto result = processor_.exportEntireBank();
        statusLabel_.setText (result.ok ? "Bank exported: " + result.presetJson.getFullPathName()
                                        : "Export failed: " + result.error,
                              juce::dontSendNotification);
    };

    reprocessButton_.onClick = [this]
    {
        if (auto* param = processor_.getApvts().getParameter (autosampler::ParameterIDs::reprocessPreset))
        {
            param->beginChangeGesture();
            param->setValueNotifyingHost (1.0f);
            param->setValueNotifyingHost (0.0f);
            param->endChangeGesture();
        }
    };

    megaBankJsonButton_.onClick = [this]
    {
        const auto dest = processor_.getExportRootDirectory().getChildFile ("mega_bank_overnight.json");
        juce::String err;
        if (processor_.writeMegaBankTemplateJson (dest, err))
            statusLabel_.setText ("Wrote mega bank template: " + dest.getFullPathName(), juce::dontSendNotification);
        else
            statusLabel_.setText ("Template failed: " + err, juce::dontSendNotification);
    };

    chooseFolderButton_.onClick = [this]
    {
        auto chooser = std::make_shared<juce::FileChooser> ("Export folder", processor_.getExportRootDirectory());
        chooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
                              [this, chooser] (const juce::FileChooser& fc)
                              {
                                  if (fc.getResult().isDirectory())
                                      processor_.setExportRootDirectory (fc.getResult());
                              });
    };

    startTimerHz (12);
}

AutoSamplerAudioProcessorEditor::~AutoSamplerAudioProcessorEditor() = default;

void AutoSamplerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1a1a22));
    g.setColour (juce::Colours::white.withAlpha (0.85f));
    g.drawFittedText ("MPC-style: Gate = note held; Release = tail after note-off; Velocity 1–4 (0 = off).",
                      getLocalBounds().reduced (12).removeFromBottom (22),
                      juce::Justification::centred,
                      2);
}

void AutoSamplerAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (10);
    titleLabel_.setBounds (area.removeFromTop (26));
    planLabel_.setBounds (area.removeFromTop (20));
    statusLabel_.setBounds (area.removeFromTop (20));
    inputMeterLabel_.setBounds (area.removeFromTop (18));
    progressBar_.setBounds (area.removeFromTop (12));
    keyboard_.setBounds (area.removeFromTop (50));
    programBox_.setBounds (area.removeFromTop (26).removeFromLeft (220));

    auto row = area.removeFromTop (34);
    megaBankJsonButton_.setBounds (row.removeFromRight (125));
    chooseFolderButton_.setBounds (row.removeFromRight (115));
    exportBankButton_.setBounds (row.removeFromRight (100));
    exportMpcKitButton_.setBounds (row.removeFromRight (110));
    exportPresetButton_.setBounds (row.removeFromRight (100));
    reprocessButton_.setBounds (row.removeFromRight (95));
    startButton_.setBounds (row.removeFromLeft (150));

    auto toggles = area.removeFromTop (26);
    trimButton_.setBounds (toggles.removeFromLeft (65));
    normalizeButton_.setBounds (toggles.removeFromLeft (90));
    loopButton_.setBounds (toggles.removeFromLeft (100));
    untilSilenceButton_.setBounds (toggles.removeFromLeft (110));

    const int rowH = 36;
    auto place = [&] (juce::Slider& s, const juce::String& name)
    {
        auto r = area.removeFromTop (rowH);
        s.setName (name);
        s.setBounds (r);
    };

    place (lowNoteSlider_, "Start note");
    place (highNoteSlider_, "End note");
    place (noteStepSlider_, "Interval (ST)");
    place (gateMsSlider_, "Gate");
    place (tailMsSlider_, "Release");
    place (prerollMsSlider_, "Preroll");
    place (pauseMsSlider_, "Pause");
    place (velocity1Slider_, "Vel 1");
    place (velocity2Slider_, "Vel 2");
    place (velocity3Slider_, "Vel 3");
    place (velocity4Slider_, "Vel 4");
    place (midiChannelSlider_, "MIDI Ch");
    place (outputGainSlider_, "Output");
}

void AutoSamplerAudioProcessorEditor::timerCallback()
{
    const auto& prog = processor_.getProgram();
    progressValue_ = static_cast<double> (processor_.autoSampleProgress());

    const auto plan = processor_.getSessionPlan();
    planLabel_.setText ("Keys: " + juce::String (plan.noteCount)
                            + "  Vels: " + juce::String (plan.velocityCount)
                            + "  Splits: " + juce::String (plan.totalCaptures)
                            + "  ~" + juce::String (plan.estimatedTotalSeconds, 1) + " s",
                        juce::dontSendNotification);

    const float inDb = juce::Decibels::gainToDecibels (processor_.inputPeakLevel(), -100.0f);
    inputMeterLabel_.setText ("Input peak: " + juce::String (inDb, 1) + " dBFS", juce::dontSendNotification);

    juce::String loopInfo;
    if (prog.zoneCount() > 0)
    {
        const auto& z = prog.zones().front();
        loopInfo = z.hasLoop() ? (" | Loop conf " + juce::String (z.loopConfidence, 2))
                               : juce::String { " | Loop: none" };
    }

    const auto exportNote = processor_.isExportInProgress() ? " | Exporting..." : (" | " + processor_.lastExportMessage());

    statusLabel_.setText ("State: " + stateToString (processor_.autoSampleState())
                              + " | Zones: " + juce::String (static_cast<int> (prog.zoneCount()))
                              + loopInfo
                              + exportNote,
                          juce::dontSendNotification);
}
