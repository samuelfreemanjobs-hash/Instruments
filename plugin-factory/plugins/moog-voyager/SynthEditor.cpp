#include "SynthEditor.h"
#include "MoogStackLineage.h"
#include "SynthParameterIds.h"
#include "VoyagerPresetBank.h"

namespace
{
const juce::Colour kPanelMetal { 0xff3a3d42 };
const juce::Colour kPanelMetalHi { 0xff5c6168 };
const juce::Colour kPanelInset { 0xff25282c };
const juce::Colour kPanelWood { 0xff6b4423 };
const juce::Colour kPanelWoodDark { 0xff3d2614 };
const juce::Colour kAccent { 0xfff4a62a };
const juce::Colour kSectionText { 0xfff0f2f5 };

void drawSection(juce::Graphics& g, juce::Rectangle<int> r, const juce::String& title)
{
    g.setColour(kPanelInset);
    g.fillRoundedRectangle(r.toFloat(), 8.0f);
    g.setColour(kPanelWood);
    g.drawRoundedRectangle(r.toFloat(), 8.0f, 2.0f);
    g.setColour(kSectionText);
    g.setFont(juce::FontOptions(14.0f, juce::Font::bold));
    g.drawText(title, r.removeFromTop(22).reduced(8, 0), juce::Justification::centredLeft);
}

void layoutKnobRow(juce::Rectangle<int> area, std::initializer_list<juce::Slider*> knobs)
{
    const int n = static_cast<int>(knobs.size());
    const int w = area.getWidth() / n;
    int i = 0;
    for (auto* k : knobs)
    {
        k->setBounds(area.removeFromLeft(w).reduced(6));
        ++i;
        juce::ignoreUnused(i);
    }
}
} // namespace

void MeterScopePanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(1.0f);
    g.setColour(kPanelInset);
    g.fillRoundedRectangle(bounds, 4.0f);

    const float peakDb = processor.getOutputPeakDb();
    const float peakNorm = juce::jmap(juce::jlimit(-48.0f, 6.0f, peakDb), -48.0f, 6.0f, 0.0f, 1.0f);
    auto meter = bounds.removeFromRight(bounds.getWidth() * 0.22f).reduced(3.0f);
    g.setColour(kPanelMetal);
    g.fillRoundedRectangle(meter, 2.0f);
    g.setColour(accent);
    const float h = meter.getHeight() * peakNorm;
    g.fillRoundedRectangle(meter.getX(), meter.getBottom() - h, meter.getWidth(), h, 2.0f);
    g.setColour(kSectionText.withAlpha(0.8f));
    g.setFont(juce::FontOptions(9.0f));
    g.drawText("OUT", meter.removeFromTop(12), juce::Justification::centred);

    std::array<float, 256> scope {};
    processor.getScopeSnapshot(scope);
    juce::Path wave;
    const float midY = bounds.getCentreY();
    for (int i = 0; i < 256; ++i)
    {
        const float x = bounds.getX() + (static_cast<float>(i) / 255.0f) * bounds.getWidth();
        const float y = midY - scope[static_cast<size_t>(i)] * bounds.getHeight() * 0.42f;
        if (i == 0)
            wave.startNewSubPath(x, y);
        else
            wave.lineTo(x, y);
    }
    g.setColour(accent.withAlpha(0.9f));
    g.strokePath(wave, juce::PathStrokeType(1.2f));
}

void MoogVoyagerAudioProcessorEditor::styleKnob(juce::Slider& s, const juce::String& name)
{
    s.setName(name);
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 18);
    s.setColour(juce::Slider::rotarySliderFillColourId, kAccent);
    s.setColour(juce::Slider::thumbColourId, kAccent.brighter(0.2f));
}

void MoogVoyagerAudioProcessorEditor::styleMixFader(juce::Slider& s, const juce::String& name)
{
    s.setName(name);
    s.setSliderStyle(juce::Slider::LinearVertical);
    s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    s.setColour(juce::Slider::backgroundColourId, kPanelInset.darker(0.15f));
    s.setColour(juce::Slider::trackColourId, kPanelMetalHi);
    s.setColour(juce::Slider::thumbColourId, kAccent);
}

void MoogVoyagerAudioProcessorEditor::styleEnvSlider(juce::Slider& s, const juce::String& name)
{
    s.setName(name);
    s.setSliderStyle(juce::Slider::LinearVertical);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 52, 14);
    s.setColour(juce::Slider::backgroundColourId, kPanelInset.darker(0.1f));
    s.setColour(juce::Slider::trackColourId, kPanelMetalHi.withAlpha(0.6f));
    s.setColour(juce::Slider::thumbColourId, kAccent);
}

void MoogVoyagerAudioProcessorEditor::applyAccentToSlider(juce::Slider& s, juce::Colour accent, bool isFader) const
{
    if (isFader)
        s.setColour(juce::Slider::thumbColourId, accent);
    else
    {
        s.setColour(juce::Slider::rotarySliderFillColourId, accent);
        s.setColour(juce::Slider::thumbColourId, accent.brighter(0.2f));
    }
}

void MoogVoyagerAudioProcessorEditor::bindCombo(juce::ComboBox& box, const juce::StringArray& items)
{
    box.addItemList(items, 1);
}

void MoogVoyagerAudioProcessorEditor::sectionLabel(juce::Label& label, const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, kAccent);
    label.setFont(juce::FontOptions(13.0f, juce::Font::bold));
}

MoogVoyagerAudioProcessorEditor::MoogVoyagerAudioProcessorEditor(MoogVoyagerAudioProcessor& p)
    : AudioProcessorEditor(&p),
      processorRef(p),
      meterScope(p)
{
    setOpaque(true);

    titleLabel.setText("VOYAGER-V", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    titleLabel.setColour(juce::Label::textColourId, kSectionText);
    titleLabel.setFont(juce::FontOptions(28.0f, juce::Font::bold));
    addAndMakeVisible(titleLabel);

    subtitleLabel.setText("FOUR-STACK MOOG ENGINE  ·  SYSTEMS ENGINEERING AUDIO", juce::dontSendNotification);
    subtitleLabel.setJustificationType(juce::Justification::centredLeft);
    subtitleLabel.setColour(juce::Label::textColourId, kSectionText.withAlpha(0.75f));
    subtitleLabel.setFont(juce::FontOptions(11.0f, juce::Font::plain));
    addAndMakeVisible(subtitleLabel);

    patchReadoutLabel.setJustificationType(juce::Justification::centred);
    patchReadoutLabel.setColour(juce::Label::textColourId, kAccent);
    patchReadoutLabel.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    addAndMakeVisible(patchReadoutLabel);

    stackBadgeLabel.setJustificationType(juce::Justification::centredLeft);
    stackBadgeLabel.setFont(juce::FontOptions(15.0f, juce::Font::bold));
    addAndMakeVisible(stackBadgeLabel);

    stackSwitchLabel.setText("STACK MODE", juce::dontSendNotification);
    stackSwitchLabel.setColour(juce::Label::textColourId, kSectionText.withAlpha(0.85f));
    stackSwitchLabel.setJustificationType(juce::Justification::centredLeft);
    stackSwitchLabel.setFont(juce::FontOptions(11.0f, juce::Font::bold));
    addAndMakeVisible(stackSwitchLabel);

    const juce::String shortNames[4] = { "VOYAGER", "SUB PHATTY", "SUB 37", "MODEL D" };
    for (int i = 0; i < moogstack::lineageCount(); ++i)
    {
        auto& btn = stackModeSwitch[static_cast<size_t>(i)];
        btn.setButtonText(shortNames[i]);
        btn.setRadioGroupId(88001);
        btn.setClickingTogglesState(true);
        btn.setColour(juce::TextButton::buttonColourId, kPanelInset);
        btn.setColour(juce::TextButton::textColourOffId, kSectionText);
        btn.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
        btn.onClick = [this, i]() { selectStackMode(i, true); };
        addAndMakeVisible(btn);
    }

    processorRef.getApvts().addParameterListener(std::string(SynthParamIDs::moogStack), this);

    presetLabel.setText("Factory preset", juce::dontSendNotification);
    presetLabel.setColour(juce::Label::textColourId, kSectionText);
    presetLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(presetLabel);

    for (int i = 0; i < voyager::getFactoryPresetCount(); ++i)
        presetBox.addItem(juce::String(voyager::getFactoryPresetName(i).data()), i + 1);
    presetBox.setSelectedItemIndex(processorRef.getCurrentProgram(), juce::dontSendNotification);
    presetBox.onChange = [this]() {
        processorRef.setCurrentProgram(presetBox.getSelectedItemIndex());
        refreshPatchReadout();
        refreshStackChrome();
    };
    addAndMakeVisible(presetBox);

    auto stepPreset = [this](int delta) {
        const int n = voyager::getFactoryPresetCount();
        int p = processorRef.getCurrentProgram() + delta;
        if (p < 0)
            p = n - 1;
        if (p >= n)
            p = 0;
        processorRef.setCurrentProgram(p);
        presetBox.setSelectedItemIndex(p, juce::dontSendNotification);
        refreshPatchReadout();
    };
    presetPrevButton.setButtonText("<");
    presetNextButton.setButtonText(">");
    presetPrevButton.setColour(juce::TextButton::buttonColourId, kPanelInset);
    presetPrevButton.setColour(juce::TextButton::textColourOffId, kSectionText);
    presetNextButton.setColour(juce::TextButton::buttonColourId, kPanelInset);
    presetNextButton.setColour(juce::TextButton::textColourOffId, kSectionText);
    presetPrevButton.onClick = [stepPreset]() { stepPreset(-1); };
    presetNextButton.onClick = [stepPreset]() { stepPreset(1); };
    addAndMakeVisible(presetPrevButton);
    addAndMakeVisible(presetNextButton);

    filterEnvRowLabel.setText("FILTER", juce::dontSendNotification);
    ampEnvRowLabel.setText("AMP", juce::dontSendNotification);
    for (auto* l : { &filterEnvRowLabel, &ampEnvRowLabel })
    {
        l->setJustificationType(juce::Justification::centredLeft);
        l->setColour(juce::Label::textColourId, kSectionText.withAlpha(0.85f));
        l->setFont(juce::FontOptions(11.0f, juce::Font::bold));
        addAndMakeVisible(*l);
    }

    oversampleLabel.setText("OS", juce::dontSendNotification);
    oversampleLabel.setColour(juce::Label::textColourId, kSectionText.withAlpha(0.85f));
    addAndMakeVisible(oversampleLabel);
    bindCombo(oversampleBox, { "Off", "2×", "4×" });
    addAndMakeVisible(oversampleBox);

    addAndMakeVisible(meterScope);

    modLfoLabel.setText("LFO", juce::dontSendNotification);
    modLfoLabel.setColour(juce::Label::textColourId, kSectionText.withAlpha(0.85f));
    modLfoLabel.setFont(juce::FontOptions(11.0f, juce::Font::bold));
    addAndMakeVisible(modLfoLabel);

    for (auto* tb : { &noisePinkButton, &mixerOdButton, &filterOdButton })
    {
        tb->setClickingTogglesState(true);
        tb->setColour(juce::ToggleButton::textColourId, kSectionText);
        tb->setColour(juce::ToggleButton::tickColourId, kAccent);
        tb->setColour(juce::ToggleButton::tickDisabledColourId, kPanelMetalHi);
        addAndMakeVisible(*tb);
    }
    noisePinkButton.setButtonText("Pink");
    mixerOdButton.setButtonText("Mix OD");
    filterOdButton.setButtonText("Filt OD");

    refreshPatchReadout();
    refreshStackChrome();

    for (auto* lab : { &oscSectionLabel, &filterSectionLabel, &envSectionLabel, &modSectionLabel })
    {
        addAndMakeVisible(*lab);
    }
    sectionLabel(oscSectionLabel, "OSCILLATOR BANK");
    sectionLabel(filterSectionLabel, "FILTER");
    sectionLabel(envSectionLabel, "FILTER & AMP ENVELOPES");
    sectionLabel(modSectionLabel, "MODULATION BUS");

    const juce::StringArray waves { "Saw", "Square", "Triangle", "Pulse" };
    const juce::StringArray octaves { "-2", "-1", "0", "+1", "+2" };

    for (int i = 0; i < 3; ++i)
    {
        oscIndexLabel[i].setText("OSC " + juce::String(i + 1), juce::dontSendNotification);
        oscIndexLabel[i].setJustificationType(juce::Justification::centred);
        oscIndexLabel[i].setColour(juce::Label::textColourId, kSectionText);
        oscOctLabel[i].setText("Octave", juce::dontSendNotification);
        oscWaveLabel[i].setText("Wave", juce::dontSendNotification);
        for (auto* l : { &oscIndexLabel[i], &oscOctLabel[i], &oscWaveLabel[i] })
        {
            l->setJustificationType(juce::Justification::centred);
            l->setColour(juce::Label::textColourId, kSectionText.withAlpha(0.9f));
            addAndMakeVisible(*l);
        }

        styleMixFader(oscLevel[i], "Mix");
        styleKnob(oscFine[i], "Fine");
        bindCombo(oscOctave[i], octaves);
        bindCombo(oscWave[i], waves);
        addAndMakeVisible(oscLevel[i]);
        addAndMakeVisible(oscFine[i]);
        addAndMakeVisible(oscOctave[i]);
        addAndMakeVisible(oscWave[i]);
    }

    styleKnob(noiseSlider, "Noise");
    styleKnob(subOscSlider, "Sub −1");
    addAndMakeVisible(noiseSlider);
    addAndMakeVisible(subOscSlider);

    styleKnob(filterCutoff, "Cutoff");
    styleKnob(filterRes, "Resonance");
    styleKnob(filterDrive, "Drive");
    styleKnob(filterEnvAmt, "F.Env");
    styleKnob(filterKb, "KB Track");
    styleEnvSlider(fAttack, "A");
    styleEnvSlider(fDecay, "D");
    styleEnvSlider(fSustain, "S");
    styleEnvSlider(fRelease, "R");
    styleEnvSlider(aAttack, "A");
    styleEnvSlider(aDecay, "D");
    styleEnvSlider(aSustain, "S");
    styleEnvSlider(aRelease, "R");
    styleKnob(lfoRate, "LFO Hz");
    styleKnob(lfoPitch, "→ Pitch");
    styleKnob(lfoFilter, "→ Filter");
    styleKnob(glide, "Glide");
    styleKnob(masterSlider, "Master");

    styleKnob(mixerDriveSlider, "Mixer");
    for (auto* s : { &filterCutoff, &filterRes, &filterDrive, &filterEnvAmt, &filterKb, &mixerDriveSlider,
                     &fAttack, &fDecay, &fSustain, &fRelease, &aAttack, &aDecay, &aSustain, &aRelease,
                     &lfoRate, &lfoPitch, &lfoFilter, &glide, &masterSlider })
        addAndMakeVisible(*s);

    auto& apvts = processorRef.getApvts();
    masterAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::outputGainDb), masterSlider);

    const std::string_view levelIds[3] = { SynthParamIDs::osc1Level, SynthParamIDs::osc2Level, SynthParamIDs::osc3Level };
    const std::string_view fineIds[3] = { SynthParamIDs::osc1Fine, SynthParamIDs::osc2Fine, SynthParamIDs::osc3Fine };
    const std::string_view octIds[3] = { SynthParamIDs::osc1Octave, SynthParamIDs::osc2Octave, SynthParamIDs::osc3Octave };
    const std::string_view waveIds[3] = { SynthParamIDs::osc1Wave, SynthParamIDs::osc2Wave, SynthParamIDs::osc3Wave };

    for (int i = 0; i < 3; ++i)
    {
        oscLevelAttachments[static_cast<size_t>(i)] =
            std::make_unique<SliderAttachment>(apvts, std::string(levelIds[i]), oscLevel[i]);
        oscFineAttachments[static_cast<size_t>(i)] =
            std::make_unique<SliderAttachment>(apvts, std::string(fineIds[i]), oscFine[i]);
        oscOctaveAttachments[static_cast<size_t>(i)] =
            std::make_unique<ComboAttachment>(apvts, std::string(octIds[i]), oscOctave[i]);
        oscWaveAttachments[static_cast<size_t>(i)] =
            std::make_unique<ComboAttachment>(apvts, std::string(waveIds[i]), oscWave[i]);
    }

    noiseAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::noiseLevel), noiseSlider);
    filterCutoffAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::filterCutoff), filterCutoff);
    filterResAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::filterResonance), filterRes);
    filterDriveAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::filterDrive), filterDrive);
    filterEnvAmtAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::filterEnvAmount), filterEnvAmt);
    filterKbAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::filterKeyTrack), filterKb);
    mixerDriveAttachment =
        std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::mixerDrive), mixerDriveSlider);
    subOscAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::subOscLevel), subOscSlider);
    noisePinkAttachment = std::make_unique<ButtonAttachment>(apvts, std::string(SynthParamIDs::noisePink), noisePinkButton);
    mixerOdAttachment = std::make_unique<ButtonAttachment>(apvts, std::string(SynthParamIDs::mixerOd), mixerOdButton);
    filterOdAttachment = std::make_unique<ButtonAttachment>(apvts, std::string(SynthParamIDs::filterOd), filterOdButton);
    oversampleAttachment = std::make_unique<ComboAttachment>(apvts, std::string(SynthParamIDs::oversample), oversampleBox);

    fAttackAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::filterAttack), fAttack);
    fDecayAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::filterDecay), fDecay);
    fSustainAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::filterSustain), fSustain);
    fReleaseAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::filterRelease), fRelease);

    aAttackAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::ampAttack), aAttack);
    aDecayAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::ampDecay), aDecay);
    aSustainAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::ampSustain), aSustain);
    aReleaseAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::ampRelease), aRelease);

    lfoRateAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::lfoRate), lfoRate);
    lfoPitchAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::lfoToPitch), lfoPitch);
    lfoFilterAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::lfoToFilter), lfoFilter);
    glideAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::glideTime), glide);

    filterCutoff.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 20);
    filterCutoff.setPopupDisplayEnabled(true, false, this);

    setResizeLimits(960, 600, 1480, 940);
    setSize(1100, 720);
    startTimerHz(28);
}

void MoogVoyagerAudioProcessorEditor::timerCallback()
{
    meterScope.repaint();
}

MoogVoyagerAudioProcessorEditor::~MoogVoyagerAudioProcessorEditor()
{
    processorRef.getApvts().removeParameterListener(std::string(SynthParamIDs::moogStack), this);
}

juce::Colour MoogVoyagerAudioProcessorEditor::accentForLineage(moogstack::Lineage lineage)
{
    switch (lineage)
    {
        case moogstack::Lineage::voyager:
            return juce::Colour(0xfff4a62a);
        case moogstack::Lineage::subPhatty:
            return juce::Colour(0xffe85d4a);
        case moogstack::Lineage::subsequent37:
            return juce::Colour(0xff4ec9e0);
        case moogstack::Lineage::modelD:
            return juce::Colour(0xff6fa8dc);
    }
    return juce::Colour(0xfff4a62a);
}

juce::Colour MoogVoyagerAudioProcessorEditor::accentForCurrentStack() const
{
    const int idx = static_cast<int>(
        processorRef.getApvts().getRawParameterValue(std::string(SynthParamIDs::moogStack))->load());
    return accentForLineage(moogstack::lineageFromChoiceIndex(idx));
}

void MoogVoyagerAudioProcessorEditor::selectStackMode(int index, bool applyTemplate)
{
    index = juce::jlimit(0, moogstack::lineageCount() - 1, index);
    if (auto* param = processorRef.getApvts().getParameter(std::string(SynthParamIDs::moogStack)))
    {
        const int n = moogstack::lineageCount();
        const float norm = n > 1 ? static_cast<float>(index) / static_cast<float>(n - 1) : 0.0f;
        param->setValueNotifyingHost(norm);
    }
    if (applyTemplate)
        moogstack::applyLineageTemplate(processorRef, moogstack::lineageFromChoiceIndex(index));
    refreshStackChrome();
}

void MoogVoyagerAudioProcessorEditor::parameterChanged(const juce::String& parameterID, float newValue)
{
    juce::ignoreUnused(newValue);
    if (parameterID == "moogStack")
        refreshStackChrome();
}

void MoogVoyagerAudioProcessorEditor::refreshStackChrome()
{
    const int idx = static_cast<int>(
        processorRef.getApvts().getRawParameterValue(std::string(SynthParamIDs::moogStack))->load());
    const auto lineage = moogstack::lineageFromChoiceIndex(idx);
    const auto accent = accentForCurrentStack();

    stackBadgeLabel.setText(juce::String(moogstack::lineageShortLabel(lineage).data()), juce::dontSendNotification);
    stackBadgeLabel.setColour(juce::Label::textColourId, accent);

    juce::String oscTitle;
    switch (lineage)
    {
        case moogstack::Lineage::voyager:
            oscTitle = "VOYAGER OSC BANK (3)";
            break;
        case moogstack::Lineage::subPhatty:
            oscTitle = "SUB PHATTY OSC + SUB";
            break;
        case moogstack::Lineage::subsequent37:
            oscTitle = "SUBSEQUENT 37 DUAL/SUB";
            break;
        case moogstack::Lineage::modelD:
            oscTitle = "MODEL D OSCILLATORS";
            break;
    }
    sectionLabel(oscSectionLabel, oscTitle);

    for (int i = 0; i < 3; ++i)
    {
        applyAccentToSlider(oscLevel[i], accent, true);
        applyAccentToSlider(oscFine[i], accent, false);
    }
    for (auto* s : { &filterCutoff, &filterRes, &subOscSlider, &mixerDriveSlider })
        applyAccentToSlider(*s, accent, false);
    for (auto* s : { &fAttack, &fDecay, &fSustain, &fRelease, &aAttack, &aDecay, &aSustain, &aRelease })
        applyAccentToSlider(*s, accent, true);
    for (auto* s : { &lfoRate, &lfoPitch, &lfoFilter, &glide, &noiseSlider, &masterSlider })
        applyAccentToSlider(*s, accent, false);

    patchReadoutLabel.setColour(juce::Label::textColourId, accent);

    for (int i = 0; i < moogstack::lineageCount(); ++i)
    {
        const auto lin = moogstack::lineageFromChoiceIndex(i);
        auto& btn = stackModeSwitch[static_cast<size_t>(i)];
        btn.setColour(juce::TextButton::buttonOnColourId, accentForLineage(lin));
        btn.setToggleState(i == idx, juce::dontSendNotification);
    }

    for (auto* tb : { &noisePinkButton, &mixerOdButton, &filterOdButton })
        tb->setColour(juce::ToggleButton::tickColourId, accent);

    meterScope.setAccentColour(accent);
    meterScope.repaint();

    repaint();
}

void MoogVoyagerAudioProcessorEditor::refreshPatchReadout()
{
    const int prog = processorRef.getCurrentProgram();
    const auto name = voyager::getFactoryPresetName(prog);
    const juce::String display =
        name.empty() ? juce::String("PATCH: —")
                     : juce::String("PATCH: PROG ") + juce::String(prog + 1).paddedLeft('0', 3) + " \""
                         + juce::String(name.data()) + "\"";
    patchReadoutLabel.setText(display, juce::dontSendNotification);
}

void MoogVoyagerAudioProcessorEditor::paint(juce::Graphics& g)
{
    const auto accent = accentForCurrentStack();
    const int woodW = 36;
    g.fillAll(kPanelWoodDark);
    g.setColour(kPanelWood);
    g.fillRect(0, 0, woodW, getHeight());
    g.fillRect(getWidth() - woodW, 0, woodW, getHeight());

    juce::Rectangle<int> panel(woodW, 0, getWidth() - 2 * woodW, getHeight());
    juce::ColourGradient metalGrad(kPanelMetalHi, panel.getTopLeft().toFloat(), kPanelMetal,
                                   panel.getBottomRight().toFloat(), false);
    g.setGradientFill(metalGrad);
    g.fillRect(panel);

    g.setColour(kPanelInset);
    g.fillRect(panel.getX(), panel.getY(), panel.getWidth(), 112);
    g.setColour(accent.withAlpha(0.55f));
    g.fillRect(panel.getX(), panel.getY() + 110, panel.getWidth(), 2);

    auto bounds = getLocalBounds().reduced(woodW + 10, 12).withTrimmedTop(122);
    auto oscBox = bounds.removeFromTop(195);
    auto filterBox = bounds.removeFromTop(148);
    auto envBox = bounds.removeFromTop(158);

    drawSection(g, oscBox, "Oscillator 1 · 2 · 3");
    drawSection(g, filterBox, "Ladder filter");
    drawSection(g, envBox, "Filter & amplifier envelopes");
    drawSection(g, bounds, "Modulation bus");
}

void MoogVoyagerAudioProcessorEditor::resized()
{
    const int woodW = 36;
    auto panel = getLocalBounds().withTrimmedLeft(woodW).withTrimmedRight(woodW);

    titleLabel.setBounds(panel.getX() + 14, 8, 280, 32);
    subtitleLabel.setBounds(panel.getX() + 14, 38, 420, 16);
    stackBadgeLabel.setBounds(panel.getX() + 14, 58, 200, 20);
    patchReadoutLabel.setBounds(panel.getCentreX() - 200, 8, 400, 22);
    oversampleLabel.setBounds(panel.getRight() - 158, 8, 22, 18);
    oversampleBox.setBounds(panel.getRight() - 134, 6, 52, 22);
    meterScope.setBounds(panel.getRight() - 220, 30, 206, 74);

  {
    auto stackRow = panel.reduced(14, 0).withTrimmedTop(52).removeFromTop(30);
    stackSwitchLabel.setBounds(stackRow.removeFromLeft(88).reduced(0, 4));
    const int btnW = stackRow.getWidth() / moogstack::lineageCount();
    for (int i = 0; i < moogstack::lineageCount(); ++i)
        stackModeSwitch[static_cast<size_t>(i)].setBounds(stackRow.removeFromLeft(btnW).reduced(i == 0 ? 0 : 2, 2));
  }

    auto presetRow = panel.reduced(14, 0).withTrimmedTop(86).removeFromTop(26);
    presetLabel.setBounds(presetRow.removeFromRight(108));
    presetNextButton.setBounds(presetRow.removeFromRight(28).reduced(2, 2));
    presetBox.setBounds(presetRow.removeFromRight(juce::jmin(340, presetRow.getWidth())).reduced(4, 0));
    presetPrevButton.setBounds(presetRow.removeFromRight(28).reduced(2, 2));

    auto bounds = panel.reduced(12).withTrimmedTop(122);
    auto oscBox = bounds.removeFromTop(195);
    oscSectionLabel.setBounds(oscBox.removeFromTop(20));
    auto oscInner = oscBox.reduced(10, 4);

    for (int i = 0; i < 3; ++i)
    {
        auto col = oscInner.removeFromLeft(oscInner.getWidth() / 3).reduced(4);
        oscIndexLabel[i].setBounds(col.removeFromTop(18));
        const int faderW = juce::jmin(36, col.getWidth() / 3);
        auto faderCol = col.removeFromLeft(faderW);
        oscLevel[i].setBounds(faderCol.reduced(2, 0));
        auto ctrlCol = col;
        oscFine[i].setBounds(ctrlCol.removeFromTop(juce::jmin(72, ctrlCol.getHeight() / 2)).reduced(4));
        oscOctLabel[i].setBounds(ctrlCol.removeFromTop(14));
        oscOctave[i].setBounds(ctrlCol.removeFromTop(22));
        oscWaveLabel[i].setBounds(ctrlCol.removeFromTop(14));
        oscWave[i].setBounds(ctrlCol.removeFromTop(22));
    }

    auto filterBox = bounds.removeFromTop(148);
    filterSectionLabel.setBounds(filterBox.removeFromTop(20));
    auto fIn = filterBox.reduced(10, 4);
    auto fRow = fIn.removeFromTop(88);
  {
    const int bigW = juce::jmin(120, fRow.getWidth() / 4);
    filterCutoff.setBounds(fRow.removeFromLeft(bigW).reduced(4));
    const int n = 6;
    const int w = fRow.getWidth() / n;
    for (auto* k : { &filterRes, &filterDrive, &filterEnvAmt, &filterKb, &mixerDriveSlider, &subOscSlider })
        k->setBounds(fRow.removeFromLeft(w).reduced(6));
  }
    auto odRow = fIn.removeFromTop(26);
    mixerOdButton.setBounds(odRow.removeFromLeft(72).reduced(2));
    filterOdButton.setBounds(odRow.removeFromLeft(72).reduced(2));

    auto envBox = bounds.removeFromTop(158);
    envSectionLabel.setBounds(envBox.removeFromTop(20));
    auto eIn = envBox.reduced(10, 4);
    filterEnvRowLabel.setBounds(eIn.removeFromTop(16));
    auto fEnvRow = eIn.removeFromTop(58);
    layoutKnobRow(fEnvRow, { &fAttack, &fDecay, &fSustain, &fRelease });
    ampEnvRowLabel.setBounds(eIn.removeFromTop(16));
    auto aEnvRow = eIn;
    layoutKnobRow(aEnvRow, { &aAttack, &aDecay, &aSustain, &aRelease });

    auto modBox = bounds.removeFromTop(112);
    modSectionLabel.setBounds(modBox.removeFromTop(20));
    auto mIn = modBox.reduced(10, 4);
    modLfoLabel.setBounds(mIn.removeFromTop(14));
    layoutKnobRow(mIn.removeFromTop(42), { &lfoRate, &lfoPitch, &lfoFilter });
    auto m2 = mIn;
    noisePinkButton.setBounds(m2.removeFromLeft(56).removeFromTop(22).reduced(0, 2));
    layoutKnobRow(m2, { &glide, &noiseSlider, &masterSlider });
}
