#include "PluginEditor.h"

namespace
{
constexpr int kW = 720;
constexpr int kH = 560;

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
    recordSecSlider_.setRange (0.25, 30.0, 0.01);
    outputGainSlider_.setRange (-24.0, 12.0, 0.01);

    for (auto* s : { &lowNoteSlider_, &highNoteSlider_, &recordSecSlider_, &outputGainSlider_ })
    {
        s->setSliderStyle (juce::Slider::LinearHorizontal);
        s->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 18);
        addAndMakeVisible (*s);
    }

    lowNoteAttachment_ = std::make_unique<SliderAttachment> (processor_.getApvts(), autosampler::ParameterIDs::lowNote, lowNoteSlider_);
    highNoteAttachment_ = std::make_unique<SliderAttachment> (processor_.getApvts(), autosampler::ParameterIDs::highNote, highNoteSlider_);
    recordSecAttachment_ = std::make_unique<SliderAttachment> (processor_.getApvts(), autosampler::ParameterIDs::recordSeconds, recordSecSlider_);
    outputGainAttachment_ = std::make_unique<SliderAttachment> (processor_.getApvts(), autosampler::ParameterIDs::outputGain, outputGainSlider_);
    startAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), autosampler::ParameterIDs::startAutoSample, startButton_);
    trimAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), autosampler::ParameterIDs::trimSilence, trimButton_);
    normalizeAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), autosampler::ParameterIDs::normalizeCapture, normalizeButton_);
    loopAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), autosampler::ParameterIDs::sustainLoop, loopButton_);
    velocityLayersAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(),
                                                                       autosampler::ParameterIDs::multiVelocityLayers,
                                                                       velocityLayersButton_);

    for (auto* b : { &startButton_, &exportPresetButton_, &exportMpcKitButton_, &exportBankButton_,
                     &chooseFolderButton_, &reprocessButton_, &megaBankJsonButton_ })
        addAndMakeVisible (*b);
    for (auto* b : { &trimButton_, &normalizeButton_, &loopButton_, &velocityLayersButton_ })
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

    startTimerHz (15);
}

AutoSamplerAudioProcessorEditor::~AutoSamplerAudioProcessorEditor() = default;

void AutoSamplerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1a1a22));
    g.setColour (juce::Colours::white.withAlpha (0.85f));
    g.drawFittedText ("Live capture: route audio in + MIDI thru. Overnight banks: use AutoSamplerMegaBank CLI + mega_bank JSON.",
                      getLocalBounds().reduced (12).removeFromBottom (24),
                      juce::Justification::centred,
                      2);
}

void AutoSamplerAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (12);
    titleLabel_.setBounds (area.removeFromTop (28));
    statusLabel_.setBounds (area.removeFromTop (22));
    inputMeterLabel_.setBounds (area.removeFromTop (18));
    progressBar_.setBounds (area.removeFromTop (14));
    keyboard_.setBounds (area.removeFromTop (54));
    programBox_.setBounds (area.removeFromTop (28).removeFromLeft (220));

    auto row = area.removeFromTop (36);
    megaBankJsonButton_.setBounds (row.removeFromRight (130));
    chooseFolderButton_.setBounds (row.removeFromRight (120));
    exportBankButton_.setBounds (row.removeFromRight (110));
    exportMpcKitButton_.setBounds (row.removeFromRight (120));
    exportPresetButton_.setBounds (row.removeFromRight (110));
    reprocessButton_.setBounds (row.removeFromRight (100));
    startButton_.setBounds (row.removeFromLeft (160));

    auto toggles = area.removeFromTop (28);
    trimButton_.setBounds (toggles.removeFromLeft (70));
    normalizeButton_.setBounds (toggles.removeFromLeft (95));
    loopButton_.setBounds (toggles.removeFromLeft (105));
    velocityLayersButton_.setBounds (toggles.removeFromLeft (110));

    auto sliders = area.removeFromTop (160);
    lowNoteSlider_.setBounds (sliders.removeFromTop (40));
    highNoteSlider_.setBounds (sliders.removeFromTop (40));
    recordSecSlider_.setBounds (sliders.removeFromTop (40));
    outputGainSlider_.setBounds (sliders.removeFromTop (40));
}

void AutoSamplerAudioProcessorEditor::timerCallback()
{
    const auto& prog = processor_.getProgram();
    progressValue_ = static_cast<double> (processor_.autoSampleProgress());

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
