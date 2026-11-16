#include "PluginEditor.h"
#include "../Presets/GenreBankLibrary.h"

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
    setSize(1120, 720);
    setResizable(true, true);
    setResizeLimits(960, 620, 1920, 1200);

    title_.setText("DiskLordz Drum Design Lab", juce::dontSendNotification);
    title_.setFont(juce::FontOptions(22.0f, juce::Font::bold));
    title_.setColour(juce::Label::textColourId, juce::Colour(0xfff4b35b));
    addAndMakeVisible(title_);

    buildModeChrome();
    buildPadButtons();
    buildEditControls();
    updateModeVisibility();
    refreshPadHighlights();
    startTimerHz(20);
}

void DrumDesignLabAudioProcessorEditor::buildModeChrome()
{
    modeBox_.addItemList({ "SYNTH", "DESIGN", "DNA", "GROOVE", "FACTORY" }, 1);
    addAndMakeVisible(modeBox_);
    modeAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        proc_.apvts, "ddl_mode", modeBox_);

    styleLabel_.setText("Kit Style", juce::dontSendNotification);
    addAndMakeVisible(styleLabel_);
    for (int i = 0; i < static_cast<int>(disklordz::KitStyle::Count); ++i)
        styleBox_.addItem(disklordz::kitStyleName(static_cast<disklordz::KitStyle>(i)), i + 1);
    addAndMakeVisible(styleBox_);
    styleAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        proc_.apvts, "ddl_kit_style", styleBox_);

    formulaLabel_.setText("DNA Formula", juce::dontSendNotification);
    addAndMakeVisible(formulaLabel_);
    for (int i = 0; i < disklordz::genre_bank::kFormulaCount; ++i)
        formulaBox_.addItem(disklordz::genre_bank::formulaAt(i).displayName, i + 1);
    addAndMakeVisible(formulaBox_);
    formulaAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        proc_.apvts, "ddl_dna_formula", formulaBox_);

    gainLabel_.setText("Output", juce::dontSendNotification);
    addAndMakeVisible(gainLabel_);
    gainSlider_.setSliderStyle(juce::Slider::LinearHorizontal);
    gainSlider_.setTextBoxStyle(juce::Slider::TextBoxRight, false, 52, 18);
    addAndMakeVisible(gainSlider_);
    gainAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        proc_.apvts, "ddl_output_gain", gainSlider_);

    masterLabel_.setText("Master dB", juce::dontSendNotification);
    addAndMakeVisible(masterLabel_);
    masterDbSlider_.setSliderStyle(juce::Slider::LinearHorizontal);
    masterDbSlider_.setTextBoxStyle(juce::Slider::TextBoxRight, false, 52, 18);
    addAndMakeVisible(masterDbSlider_);
    masterAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        proc_.apvts, "ddl_master_gain_db", masterDbSlider_);

    addAndMakeVisible(bypassToggle_);
    bypassAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        proc_.apvts, "ddl_bypass", bypassToggle_);

    statusLabel_.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(statusLabel_);

    editTitle_.setText("Sound Edit", juce::dontSendNotification);
    editTitle_.setFont(juce::FontOptions(16.0f, juce::Font::bold));
    editTitle_.setColour(juce::Label::textColourId, juce::Colour(0xfff4b35b));
    addAndMakeVisible(editTitle_);

    destructionBox_.addItemList({ "Clean", "Mixed", "Destroyed" }, 1);
    addAndMakeVisible(destructionBox_);
    destructionAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        proc_.apvts, "ddl_destruction_mode", destructionBox_);

    mutateBtn_.onClick = [this] { proc_.mutateSelectedPad(); };
    addAndMakeVisible(mutateBtn_);
    mutateStrength_.setSliderStyle(juce::Slider::LinearHorizontal);
    mutateStrength_.setTextBoxStyle(juce::Slider::TextBoxRight, false, 48, 16);
    addAndMakeVisible(mutateStrength_);
    mutateAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        proc_.apvts, "ddl_mutate_strength", mutateStrength_);

    // DNA panel
    dnaTitle_.setText("Hyper DNA Morph", juce::dontSendNotification);
    dnaTitle_.setFont(juce::FontOptions(16.0f, juce::Font::bold));
    dnaTitle_.setColour(juce::Label::textColourId, juce::Colour(0xffb48cff));
    addAndMakeVisible(dnaTitle_);
    dnaMatrixBox_.addItemList({ "HyperDNA", "PercussionDNA", "ZayDNA", "SouthDNA" }, 1);
    addAndMakeVisible(dnaMatrixBox_);
    dnaMatrixAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        proc_.apvts, "ddl_dna_matrix", dnaMatrixBox_);
    for (auto* s : { &dnaX_, &dnaY_, &dnaInfl_ })
    {
        s->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 56, 16);
        s->setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffb48cff));
        addAndMakeVisible(*s);
    }
    dnaXAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        proc_.apvts, "ddl_dna_x", dnaX_);
    dnaYAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        proc_.apvts, "ddl_dna_y", dnaY_);
    dnaInflAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        proc_.apvts, "ddl_dna_influence", dnaInfl_);
    applyDnaBtn_.onClick = [this] {
        proc_.applyHyperDnaFromApvts();
        pullKnobsIfNeeded();
    };
    addAndMakeVisible(applyDnaBtn_);

    // Groove panel
    grooveTitle_.setText("Groove Sequencer", juce::dontSendNotification);
    grooveTitle_.setFont(juce::FontOptions(16.0f, juce::Font::bold));
    grooveTitle_.setColour(juce::Label::textColourId, juce::Colour(0xff7ec8ff));
    addAndMakeVisible(grooveTitle_);
    addAndMakeVisible(seqToggle_);
    seqAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        proc_.apvts, "ddl_seq_enabled", seqToggle_);
    grooveBox_.addItemList({ "Milli Pressure", "Dirty Digital", "Boss Bounce", "Southern Oddity",
                             "Raw Machine", "Minimal Monster", "Eccentric Atlanta", "Mechanical Bounce",
                             "Digital Funk", "Experimental Street", "Sparse Knock", "Rhythmic Hypnosis" },
                           1);
    addAndMakeVisible(grooveBox_);
    grooveAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        proc_.apvts, "ddl_groove_family", grooveBox_);
    bpmSlider_.setSliderStyle(juce::Slider::LinearHorizontal);
    bpmSlider_.setTextBoxStyle(juce::Slider::TextBoxRight, false, 48, 16);
    addAndMakeVisible(bpmSlider_);
    bpmAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        proc_.apvts, "ddl_bpm", bpmSlider_);
    swingSlider_.setSliderStyle(juce::Slider::LinearHorizontal);
    swingSlider_.setTextBoxStyle(juce::Slider::TextBoxRight, false, 48, 16);
    addAndMakeVisible(swingSlider_);
    swingAttach_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        proc_.apvts, "ddl_swing", swingSlider_);

    // Factory panel
    factoryTitle_.setText("Kit Sample Factory", juce::dontSendNotification);
    factoryTitle_.setFont(juce::FontOptions(16.0f, juce::Font::bold));
    factoryTitle_.setColour(juce::Label::textColourId, juce::Colour(0xff8fd49a));
    addAndMakeVisible(factoryTitle_);
    exportCurrentBtn_.onClick = [this] { exportCurrentKit(); };
    exportAllBtn_.onClick = [this] { exportAllKits(); };
    addAndMakeVisible(exportCurrentBtn_);
    addAndMakeVisible(exportAllBtn_);
}

void DrumDesignLabAudioProcessorEditor::buildEditControls()
{
    for (size_t i = 0; i < knobs_.size(); ++i)
    {
        auto& k = knobs_[i];
        k.slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        k.slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 16);
        k.slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xfff4b35b));
        k.label.setText(kKnobSpecs[i].label, juce::dontSendNotification);
        k.label.setJustificationType(juce::Justification::centred);
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
    const char* mode = proc_.engineUsesThreeLayerPads() ? "pad recipe" : "engine voice";
    editTitle_.setText(juce::String("Sound Edit — ") + names[pad] + " (" + mode + ")",
                       juce::dontSendNotification);
    refreshPadHighlights();
}

void DrumDesignLabAudioProcessorEditor::updateModeVisibility()
{
    const auto mode = proc_.getWorkspaceMode();
    const int mi = static_cast<int>(mode);
    if (mi == lastMode_)
        return;
    lastMode_ = mi;

    const bool synthOrDesign = mode == DrumDesignLabAudioProcessor::WorkspaceMode::Synth
                               || mode == DrumDesignLabAudioProcessor::WorkspaceMode::Design;
    const bool dna = mode == DrumDesignLabAudioProcessor::WorkspaceMode::Dna;
    const bool groove = mode == DrumDesignLabAudioProcessor::WorkspaceMode::Groove;
    const bool factory = mode == DrumDesignLabAudioProcessor::WorkspaceMode::Factory;

    for (auto& k : knobs_)
    {
        k.slider.setVisible(synthOrDesign || dna);
        k.label.setVisible(synthOrDesign || dna);
    }
    destructionBox_.setVisible(synthOrDesign);
    mutateBtn_.setVisible(synthOrDesign || dna);
    mutateStrength_.setVisible(synthOrDesign || dna);

    dnaTitle_.setVisible(dna);
    dnaMatrixBox_.setVisible(dna);
    dnaX_.setVisible(dna);
    dnaY_.setVisible(dna);
    dnaInfl_.setVisible(dna);
    applyDnaBtn_.setVisible(dna);

    grooveTitle_.setVisible(groove);
    seqToggle_.setVisible(groove);
    grooveBox_.setVisible(groove);
    bpmSlider_.setVisible(groove);
    swingSlider_.setVisible(groove);

    factoryTitle_.setVisible(factory);
    exportCurrentBtn_.setVisible(factory);
    exportAllBtn_.setVisible(factory);
    resized();
}

void DrumDesignLabAudioProcessorEditor::exportCurrentKit()
{
    auto chooser = std::make_shared<juce::FileChooser>("Export current kit pack",
                                                       juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
                                                       "*");
    chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
                         [this, chooser](const juce::FileChooser& fc) {
                             auto dir = fc.getResult();
                             if (dir == juce::File())
                                 return;
                             const int vars = static_cast<int>(proc_.apvts.getRawParameterValue("ddl_factory_variations")->load());
                             proc_.exportKitPack(dir, false, vars);
                         });
}

void DrumDesignLabAudioProcessorEditor::exportAllKits()
{
    auto chooser = std::make_shared<juce::FileChooser>("Export all 32 DNA kit packs",
                                                       juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
                                                       "*");
    chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
                         [this, chooser](const juce::FileChooser& fc) {
                             auto dir = fc.getResult();
                             if (dir == juce::File())
                                 return;
                             const int vars = static_cast<int>(proc_.apvts.getRawParameterValue("ddl_factory_variations")->load());
                             proc_.exportKitPack(dir, true, vars);
                         });
}

void DrumDesignLabAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff121214));
    g.setColour(juce::Colour(0xff1c1c22));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(8.0f), 10.0f);
}

void DrumDesignLabAudioProcessorEditor::layoutKnob(juce::Slider& s, juce::Label& l, juce::Rectangle<int> area)
{
    l.setBounds(area.removeFromTop(18));
    s.setBounds(area.reduced(4));
}

void DrumDesignLabAudioProcessorEditor::resized()
{
    auto r = getLocalBounds().reduced(14);
    title_.setBounds(r.removeFromTop(28));
    r.removeFromTop(4);
    auto top = r.removeFromTop(28);
    modeBox_.setBounds(top.removeFromLeft(120));
    top.removeFromLeft(8);
    styleLabel_.setBounds(top.removeFromLeft(60));
    styleBox_.setBounds(top.removeFromLeft(220));
    top.removeFromLeft(8);
    formulaLabel_.setBounds(top.removeFromLeft(80));
    formulaBox_.setBounds(top.removeFromLeft(220));
    top.removeFromLeft(8);
    gainLabel_.setBounds(top.removeFromLeft(48));
    gainSlider_.setBounds(top.removeFromLeft(140));
    masterLabel_.setBounds(top.removeFromLeft(70));
    masterDbSlider_.setBounds(top.removeFromLeft(120));
    bypassToggle_.setBounds(top.removeFromLeft(80));

    r.removeFromTop(4);
    statusLabel_.setBounds(r.removeFromTop(20));
    r.removeFromTop(6);

    auto left = r.removeFromLeft(r.getWidth() / 2).reduced(4);
    auto right = r.reduced(4);

    editTitle_.setBounds(right.removeFromTop(22));
    auto destRow = right.removeFromTop(26);
    destructionBox_.setBounds(destRow.removeFromLeft(120));
    destRow.removeFromLeft(8);
    mutateBtn_.setBounds(destRow.removeFromLeft(110));
    mutateStrength_.setBounds(destRow);

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

    const auto mode = proc_.getWorkspaceMode();
    if (mode == DrumDesignLabAudioProcessor::WorkspaceMode::Dna)
    {
        dnaTitle_.setBounds(right.removeFromTop(22));
        dnaMatrixBox_.setBounds(right.removeFromTop(26));
        right.removeFromTop(4);
        auto dnaRow = right.removeFromTop(120);
        const int w = dnaRow.getWidth() / 3;
        dnaX_.setBounds(dnaRow.removeFromLeft(w).reduced(6));
        dnaY_.setBounds(dnaRow.removeFromLeft(w).reduced(6));
        dnaInfl_.setBounds(dnaRow.reduced(6));
        applyDnaBtn_.setBounds(right.removeFromTop(28).removeFromLeft(160));
        right.removeFromTop(8);
    }
    else if (mode == DrumDesignLabAudioProcessor::WorkspaceMode::Groove)
    {
        grooveTitle_.setBounds(right.removeFromTop(22));
        seqToggle_.setBounds(right.removeFromTop(26).removeFromLeft(140));
        grooveBox_.setBounds(right.removeFromTop(28));
        right.removeFromTop(6);
        bpmSlider_.setBounds(right.removeFromTop(28));
        swingSlider_.setBounds(right.removeFromTop(28));
    }
    else if (mode == DrumDesignLabAudioProcessor::WorkspaceMode::Factory)
    {
        factoryTitle_.setBounds(right.removeFromTop(22));
        right.removeFromTop(8);
        exportCurrentBtn_.setBounds(right.removeFromTop(36));
        right.removeFromTop(8);
        exportAllBtn_.setBounds(right.removeFromTop(36));
    }

    if (mode == DrumDesignLabAudioProcessor::WorkspaceMode::Synth
        || mode == DrumDesignLabAudioProcessor::WorkspaceMode::Design
        || mode == DrumDesignLabAudioProcessor::WorkspaceMode::Dna)
    {
        const int kCols = 5;
        const int kRows = 3;
        const int kW = right.getWidth() / kCols;
        const int kH = juce::jmax(90, right.getHeight() / kRows);
        for (size_t i = 0; i < knobs_.size(); ++i)
        {
            const int c = static_cast<int>(i) % kCols;
            const int rw = static_cast<int>(i) / kCols;
            auto cell = juce::Rectangle<int>(right.getX() + c * kW, right.getY() + rw * kH, kW, kH).reduced(4);
            layoutKnob(knobs_[i].slider, knobs_[i].label, cell);
        }
    }
}

void DrumDesignLabAudioProcessorEditor::timerCallback()
{
    updateModeVisibility();
    pullKnobsIfNeeded();
    statusLabel_.setText(proc_.getStatusLine(), juce::dontSendNotification);
}
