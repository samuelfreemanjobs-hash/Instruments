#include "SynthEditor.h"
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

void MoogVoyagerAudioProcessorEditor::styleKnob(juce::Slider& s, const juce::String& name)
{
    s.setName(name);
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 18);
    s.setColour(juce::Slider::rotarySliderFillColourId, kAccent);
    s.setColour(juce::Slider::thumbColourId, kAccent.brighter(0.2f));
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
      processorRef(p)
{
    setOpaque(true);

    titleLabel.setText("VOYAGER-V", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    titleLabel.setColour(juce::Label::textColourId, kSectionText);
    titleLabel.setFont(juce::FontOptions(28.0f, juce::Font::bold));
    addAndMakeVisible(titleLabel);

    subtitleLabel.setText("SYSTEMS ENGINEERING AUDIO", juce::dontSendNotification);
    subtitleLabel.setJustificationType(juce::Justification::centredLeft);
    subtitleLabel.setColour(juce::Label::textColourId, kSectionText.withAlpha(0.75f));
    subtitleLabel.setFont(juce::FontOptions(11.0f, juce::Font::plain));
    addAndMakeVisible(subtitleLabel);

    patchReadoutLabel.setJustificationType(juce::Justification::centred);
    patchReadoutLabel.setColour(juce::Label::textColourId, kAccent);
    patchReadoutLabel.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    addAndMakeVisible(patchReadoutLabel);

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
    };
    addAndMakeVisible(presetBox);
    refreshPatchReadout();

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

        styleKnob(oscLevel[i], "Level");
        styleKnob(oscFine[i], "Fine");
        bindCombo(oscOctave[i], octaves);
        bindCombo(oscWave[i], waves);
        addAndMakeVisible(oscLevel[i]);
        addAndMakeVisible(oscFine[i]);
        addAndMakeVisible(oscOctave[i]);
        addAndMakeVisible(oscWave[i]);
    }

    styleKnob(noiseSlider, "Noise");
    addAndMakeVisible(noiseSlider);

    styleKnob(filterCutoff, "Cutoff");
    styleKnob(filterRes, "Resonance");
    styleKnob(filterDrive, "Drive");
    styleKnob(filterEnvAmt, "F.Env");
    styleKnob(filterKb, "KB Track");
    styleKnob(fAttack, "F.A");
    styleKnob(fDecay, "F.D");
    styleKnob(fSustain, "F.S");
    styleKnob(fRelease, "F.R");
    styleKnob(aAttack, "A.A");
    styleKnob(aDecay, "A.D");
    styleKnob(aSustain, "A.S");
    styleKnob(aRelease, "A.R");
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

    setResizeLimits(960, 540, 1480, 860);
    setSize(1100, 640);
}

MoogVoyagerAudioProcessorEditor::~MoogVoyagerAudioProcessorEditor() = default;

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
    g.fillRect(panel.getX(), panel.getY(), panel.getWidth(), 78);
    g.setColour(kAccent.withAlpha(0.45f));
    g.fillRect(panel.getX(), panel.getY() + 76, panel.getWidth(), 2);

    auto bounds = getLocalBounds().reduced(woodW + 10, 12).withTrimmedTop(88);
    auto oscBox = bounds.removeFromTop(175);
    auto filterBox = bounds.removeFromTop(125);
    auto envBox = bounds.removeFromTop(115);

    drawSection(g, oscBox, "Oscillator 1 · 2 · 3");
    drawSection(g, filterBox, "Ladder filter");
    drawSection(g, envBox, "Filter & amplifier envelopes");
    drawSection(g, bounds, "Modulation bus");
}

void MoogVoyagerAudioProcessorEditor::resized()
{
    const int woodW = 36;
    auto panel = getLocalBounds().withTrimmedLeft(woodW).withTrimmedRight(woodW);

    titleLabel.setBounds(panel.getX() + 14, 10, 280, 32);
    subtitleLabel.setBounds(panel.getX() + 14, 40, 300, 16);
    patchReadoutLabel.setBounds(panel.getCentreX() - 220, 12, 440, 22);

    auto presetRow = panel.reduced(14, 0).withTrimmedTop(52).removeFromTop(26);
    presetLabel.setBounds(presetRow.removeFromRight(120));
    presetBox.setBounds(presetRow.removeFromRight(juce::jmin(380, presetRow.getWidth())).reduced(4, 0));

    auto bounds = panel.reduced(12).withTrimmedTop(88);
    auto oscBox = bounds.removeFromTop(175);
    oscSectionLabel.setBounds(oscBox.removeFromTop(20));
    auto oscInner = oscBox.reduced(10, 4);

    for (int i = 0; i < 3; ++i)
    {
        auto col = oscInner.removeFromLeft(oscInner.getWidth() / 3).reduced(4);
        oscIndexLabel[i].setBounds(col.removeFromTop(18));
        auto knobs = col.removeFromTop(col.getHeight() - 52);
        oscLevel[i].setBounds(knobs.removeFromLeft(knobs.getWidth() / 2).reduced(2));
        oscFine[i].setBounds(knobs.reduced(2));
        oscOctLabel[i].setBounds(col.removeFromTop(14));
        oscOctave[i].setBounds(col.removeFromTop(22));
        oscWaveLabel[i].setBounds(col.removeFromTop(14));
        oscWave[i].setBounds(col.removeFromTop(22));
    }

    auto filterBox = bounds.removeFromTop(125);
    filterSectionLabel.setBounds(filterBox.removeFromTop(20));
    auto fIn = filterBox.reduced(10, 4);
    auto fRow = fIn.removeFromTop(88);
  {
    const int bigW = juce::jmin(120, fRow.getWidth() / 4);
    filterCutoff.setBounds(fRow.removeFromLeft(bigW).reduced(4));
    const int n = 5;
    const int w = fRow.getWidth() / n;
    for (auto* k : { &filterRes, &filterDrive, &filterEnvAmt, &filterKb, &mixerDriveSlider })
        k->setBounds(fRow.removeFromLeft(w).reduced(6));
  }

    auto envBox = bounds.removeFromTop(115);
    envSectionLabel.setBounds(envBox.removeFromTop(20));
    auto eIn = envBox.reduced(10, 4);
    layoutKnobRow(eIn.removeFromTop(38), { &fAttack, &fDecay, &fSustain, &fRelease });
    layoutKnobRow(eIn, { &aAttack, &aDecay, &aSustain, &aRelease });

    auto modBox = bounds.removeFromTop(95);
    modSectionLabel.setBounds(modBox.removeFromTop(20));
    auto mIn = modBox.reduced(10, 4);
    layoutKnobRow(mIn, { &lfoRate, &lfoPitch, &lfoFilter, &glide, &noiseSlider, &masterSlider });
}
