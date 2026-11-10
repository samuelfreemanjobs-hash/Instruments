#include "PluginEditor.h"
#include "PluginEditorFormatters.h"
#include "PresetLibrary.h"

TrapLordSynthAudioProcessorEditor::TrapLordSynthAudioProcessorEditor (TrapLordSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    constexpr const char* ids[] = { "attack", "decay", "sustain", "release", "cutoff", "gain",
                                    "drive", "detune", "reverb", "delay", "arpRate", "arpGate" };
    constexpr const char* names[] = { "ATTACK", "DECAY", "SUSTAIN", "RELEASE", "CUTOFF", "LEVEL",
                                      "DRIVE", "DETUNE", "REVERB", "DELAY", "ARP DIV", "GATE" };
    for (int i = 0; i < count; ++i)
    {
        knobs[static_cast<size_t> (i)].setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        knobs[static_cast<size_t> (i)].setTextBoxStyle (juce::Slider::TextBoxBelow, false, 75, 18);
        addAndMakeVisible (knobs[static_cast<size_t> (i)]);
        labels[static_cast<size_t> (i)].setText (names[i], juce::dontSendNotification);
        labels[static_cast<size_t> (i)].setJustificationType (juce::Justification::centred);
        labels[static_cast<size_t> (i)].setColour (juce::Label::textColourId, juce::Colours::orange);
        addAndMakeVisible (labels[static_cast<size_t> (i)]);
        attachments[static_cast<size_t> (i)] = std::make_unique<SA> (p.state, ids[i], knobs[static_cast<size_t> (i)]);
    }

    TrapLordEditorFormatters::configureTimeSeconds (knobs[0]);
    TrapLordEditorFormatters::configureTimeSeconds (knobs[1]);
    TrapLordEditorFormatters::configurePercent (knobs[2]);
    TrapLordEditorFormatters::configureTimeSeconds (knobs[3]);
    TrapLordEditorFormatters::configureHz (knobs[4]);
    TrapLordEditorFormatters::configureGainDb (knobs[5]);
    TrapLordEditorFormatters::configureDrive (knobs[6]);
    TrapLordEditorFormatters::configureCents (knobs[7]);
    TrapLordEditorFormatters::configurePercent (knobs[8]);
    TrapLordEditorFormatters::configurePercent (knobs[9]);
    TrapLordEditorFormatters::configureArpDivision (knobs[10]);
    TrapLordEditorFormatters::configurePercent (knobs[11]);

    constexpr const char* macroIds[] = { "macroDarkness", "macroDirt", "macroMovement", "macroSpace" };
    constexpr const char* macroNames[] = { "DARKNESS", "DIRT", "MOVEMENT", "SPACE" };
    for (int i = 0; i < macroCount; ++i)
    {
        macroKnobs[static_cast<size_t> (i)].setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        macroKnobs[static_cast<size_t> (i)].setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 18);
        macroKnobs[static_cast<size_t> (i)].setColour (juce::Slider::rotarySliderFillColourId, juce::Colours::gold);
        addAndMakeVisible (macroKnobs[static_cast<size_t> (i)]);
        macroLabels[static_cast<size_t> (i)].setText (macroNames[i], juce::dontSendNotification);
        macroLabels[static_cast<size_t> (i)].setJustificationType (juce::Justification::centred);
        macroLabels[static_cast<size_t> (i)].setColour (juce::Label::textColourId, juce::Colours::gold);
        addAndMakeVisible (macroLabels[static_cast<size_t> (i)]);
        macroAttachments[static_cast<size_t> (i)] = std::make_unique<SA> (p.state, macroIds[i], macroKnobs[static_cast<size_t> (i)]);
        TrapLordEditorFormatters::configurePercent (macroKnobs[static_cast<size_t> (i)]);
    }

    engine.addItemList ({ "Brass", "Organ", "Strings", "FM Bell", "Bass", "Choir", "Lead", "Multisample",
                           "Analog Pulse", "Bright EP", "Haunted FM" },
                        1);
    chord.addItemList ({ "Off", "Minor", "Minor 7", "Diminished", "Sus 2", "Minor 9" }, 1);
    bankMode.addItemList ({ "Church Keys", "Dark Brass", "Trap Orchestra" }, 1);
    categoryFilter.addItem ("All categories", 1);
    for (const auto& cat : PresetLibrary::instance().categories())
        categoryFilter.addItem (cat, categoryFilter.getNumItems() + 1);
    categoryFilter.setSelectedId (1, juce::dontSendNotification);
    categoryFilter.onChange = [this] { refreshFilteredIndices(); };

    engineAttachment = std::make_unique<CA> (p.state, "engine", engine);
    chordAttachment = std::make_unique<CA> (p.state, "chord", chord);
    bankAttachment = std::make_unique<CA> (p.state, "bankMode", bankMode);
    bankModeLabel.setText ("MODE", juce::dontSendNotification);
    engineLabel.setText ("ENGINE", juce::dontSendNotification);
    chordLabel.setText ("CHORD", juce::dontSendNotification);
    sampleMapLabel.setText ("SAMPLE MAP", juce::dontSendNotification);
    for (auto* lab : { &bankModeLabel, &engineLabel, &chordLabel, &sampleMapLabel })
    {
        lab->setJustificationType (juce::Justification::centredLeft);
        lab->setColour (juce::Label::textColourId, juce::Colours::grey);
        lab->setFont (juce::Font (11.f));
        addAndMakeVisible (lab);
    }

    addAndMakeVisible (engine);
    addAndMakeVisible (chord);
    addAndMakeVisible (bankMode);
    addAndMakeVisible (categoryFilter);

    searchBox.setTextToShowWhenEmpty ("Search presets", juce::Colours::grey);
    searchBox.onTextChange = [this] { refreshFilteredIndices(); };
    addAndMakeVisible (searchBox);

    presetTitle.setJustificationType (juce::Justification::centredLeft);
    presetTitle.setColour (juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (presetTitle);

    prevBtn.onClick = [this]
    {
        if (filteredPresetIndices.isEmpty())
            return;
        listIndex = (listIndex - 1 + filteredPresetIndices.size()) % filteredPresetIndices.size();
        showPresetAtListIndex (listIndex);
    };
    nextBtn.onClick = [this]
    {
        if (filteredPresetIndices.isEmpty())
            return;
        listIndex = (listIndex + 1) % filteredPresetIndices.size();
        showPresetAtListIndex (listIndex);
    };
    addAndMakeVisible (prevBtn);
    addAndMakeVisible (nextBtn);

    exportMpc.onClick = [this] { startMpcExport(); };
    addAndMakeVisible (exportMpc);
    addAndMakeVisible (exportProgress);

    arpAttachment = std::make_unique<BA> (p.state, "arp", arp);
    addAndMakeVisible (arp);

    load.onClick = [this]
    {
        auto chooser = std::make_shared<juce::FileChooser> ("Open keygroup.json", juce::File {}, "*.json");
        chooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                              [this, chooser] (const juce::FileChooser& fc)
                              {
                                  status.setText (processor.loadSampleMap (fc.getResult()) ? "Loaded sample map"
                                                                                            : "Invalid sample map",
                                                  juce::dontSendNotification);
                              });
    };
    addAndMakeVisible (load);

    addAndMakeVisible (status);
    status.setColour (juce::Label::textColourId, juce::Colours::lightgrey);

    exportWorker = std::make_unique<KeygroupExportWorker> (processor);
    refreshFilteredIndices();
    startTimerHz (12);
    setSize (1020, 600);
    maybeRunScreenshotCapture();
}

void TrapLordSynthAudioProcessorEditor::selectCategoryAndShowFirst (const juce::String& category)
{
    for (int i = 1; i < categoryFilter.getNumItems(); ++i)
    {
        if (categoryFilter.getItemText (i) == category)
        {
            categoryFilter.setSelectedItemIndex (i, juce::dontSendNotification);
            refreshFilteredIndices();
            return;
        }
    }
    categoryFilter.setSelectedId (1, juce::dontSendNotification);
    refreshFilteredIndices();
}

void TrapLordSynthAudioProcessorEditor::maybeRunScreenshotCapture()
{
    const juce::String path = juce::SystemStats::getEnvironmentVariable ("TRAP_LORD_SCREENSHOT_PATH", {});
    if (path.isEmpty())
        return;

    const juce::String category = juce::SystemStats::getEnvironmentVariable ("TRAP_LORD_SCREENSHOT_CATEGORY", {});

    juce::Timer::callAfterDelay (
        900,
        [this, path, category]() mutable
        {
            if (category.isNotEmpty())
                selectCategoryAndShowFirst (category);

            refreshFilteredIndices();
            repaint();

            if (auto* top = getTopLevelComponent())
            {
                top->repaint();
                const auto bounds = top->getLocalBounds();
                juce::Image img (juce::Image::ARGB, bounds.getWidth(), bounds.getHeight(), true);
                juce::Graphics g (img);
                top->paintEntireComponent (g, false);

                const juce::File out (path);
                out.deleteFile();
                out.getParentDirectory().createDirectory();

                juce::PNGImageFormat png;
                std::unique_ptr<juce::FileOutputStream> stream (out.createOutputStream());
                if (stream != nullptr && stream->openedOk())
                {
                    png.writeImageToStream (img, *stream);
                    stream->flush();
                    stream.reset();
                }

                juce::Timer::callAfterDelay (50, []
                                             {
                                                 if (auto* app = juce::JUCEApplicationBase::getInstance())
                                                     app->systemRequestedQuit();
                                             });
                return;
            }

            if (auto* app = juce::JUCEApplicationBase::getInstance())
                app->systemRequestedQuit();
        });
}

juce::String TrapLordSynthAudioProcessorEditor::activeCategoryFilter() const
{
    if (categoryFilter.getSelectedId() <= 1)
        return {};
    const int idx = categoryFilter.getSelectedItemIndex();
    return idx >= 0 ? categoryFilter.getItemText (idx) : juce::String();
}

TrapLordSynthAudioProcessorEditor::~TrapLordSynthAudioProcessorEditor()
{
    stopTimer();
    if (exportWorker != nullptr)
    {
        exportWorker->requestStop();
        exportWorker->stopThread (8000);
    }
}

void TrapLordSynthAudioProcessorEditor::refreshFilteredIndices()
{
    const juce::String cat = activeCategoryFilter();
    filteredPresetIndices = PresetLibrary::instance().search (searchBox.getText(), cat);
    listIndex = 0;
    if (! filteredPresetIndices.isEmpty())
        showPresetAtListIndex (0);
    else
        presetTitle.setText ("No presets match filter", juce::dontSendNotification);
}

void TrapLordSynthAudioProcessorEditor::showPresetAtListIndex (int idx)
{
    if (! juce::isPositiveAndBelow (idx, filteredPresetIndices.size()))
        return;
    listIndex = idx;
    const int preset = filteredPresetIndices.getReference (idx);
    processor.loadFactoryPreset (preset);
    presetTitle.setText (juce::String (idx + 1) + "/" + juce::String (filteredPresetIndices.size()) + "  "
                         + PresetLibrary::instance().get (preset).name,
                         juce::dontSendNotification);
}

void TrapLordSynthAudioProcessorEditor::startMpcExport()
{
    if (exportWorker != nullptr && exportWorker->isThreadRunning())
    {
        exportWorker->requestStop();
        status.setText ("Cancelling export...", juce::dontSendNotification);
        return;
    }

    auto chooser = std::make_shared<juce::FileChooser> ("Choose folder for MPC keygroup export");
    chooser->launchAsync (juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectDirectories,
                          [this, chooser] (const juce::FileChooser& fc)
                          {
                              if (! fc.getResult().exists())
                                  return;

                              KeygroupExportOptions opts;
                              const int preset = processor.getCurrentPresetIndex();
                              opts.programName = PresetLibrary::instance().get (preset).name;
                              opts.lowNote = 24;
                              opts.highNote = 84;
                              opts.semitoneStep = 3;
                              opts.velocityLayers = 2;
                              opts.use24Bit = true;
                              progressValue = 0;
                              exportMpc.setEnabled (false);

                              exportWorker->startExport (
                                  fc.getResult(),
                                  opts,
                                  [this] (float p, const juce::String& msg)
                                  {
                                      juce::MessageManager::callAsync ([this, p, msg]
                                                                       {
                                                                           progressValue = double (p);
                                                                           status.setText (msg, juce::dontSendNotification);
                                                                       });
                                  });
                          });
}

void TrapLordSynthAudioProcessorEditor::timerCallback()
{
    if (exportWorker == nullptr)
        return;

    if (! exportWorker->isThreadRunning())
    {
        if (! exportMpc.isEnabled())
        {
            exportMpc.setEnabled (true);
            progressValue = exportWorker->succeeded() ? 1. : 0.;
            status.setText (exportWorker->resultMessage(), juce::dontSendNotification);
        }
    }
}

void TrapLordSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff101015));
    g.setColour (juce::Colours::orange);
    g.setFont (juce::Font (juce::FontOptions (28.f, juce::Font::bold)));
    g.drawText ("DISKLORDZ  /  TRAP LORD SYNTH", 22, 10, 900, 36, juce::Justification::centredLeft);
    g.setColour (juce::Colours::grey);
    g.setFont (13.f);
    g.drawText ("ATL CLASSICS  |  500 PRESETS  |  MPC KEYGROUP EXPORT", 24, 48, 900, 22, juce::Justification::centredLeft);
    g.setColour (juce::Colours::gold);
    g.drawText ("GLOBAL MACROS", 24, 418, 200, 18, juce::Justification::centredLeft);
}

void TrapLordSynthAudioProcessorEditor::resized()
{
    for (int i = 0; i < count; ++i)
    {
        const int x = 18 + (i % 6) * 151;
        const int y = 78 + (i / 6) * 145;
        labels[static_cast<size_t> (i)].setBounds (x, y, 140, 22);
        knobs[static_cast<size_t> (i)].setBounds (x, y + 24, 140, 110);
    }
    for (int i = 0; i < macroCount; ++i)
    {
        const int x = 18 + i * 118;
        macroLabels[static_cast<size_t> (i)].setBounds (x, 432, 108, 18);
        macroKnobs[static_cast<size_t> (i)].setBounds (x, 450, 108, 96);
    }

    searchBox.setBounds (20, 358, 200, 26);
    categoryFilter.setBounds (230, 358, 145, 26);
    prevBtn.setBounds (385, 358, 36, 26);
    nextBtn.setBounds (425, 358, 36, 26);
    presetTitle.setBounds (470, 358, 430, 26);
    exportMpc.setBounds (20, 390, 165, 28);
    exportProgress.setBounds (195, 395, 300, 18);
    arp.setBounds (900, 358, 85, 26);

    bankModeLabel.setBounds (510, 372, 155, 16);
    engineLabel.setBounds (675, 372, 155, 16);
    chordLabel.setBounds (840, 372, 140, 16);
    bankMode.setBounds (510, 390, 155, 28);
    engine.setBounds (675, 390, 155, 28);
    chord.setBounds (840, 390, 140, 28);

    sampleMapLabel.setBounds (510, 422, 155, 16);
    load.setBounds (510, 440, 165, 28);
    status.setBounds (20, 568, 970, 22);
}
