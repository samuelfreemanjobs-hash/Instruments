#include "SynthEditor.h"

namespace
{
const juce::Colour kPanelBlue { 0xff1a2744 };
const juce::Colour kPanelWood { 0xff3d2817 };
const juce::Colour kAccent { 0xffe8c547 };

void layoutRow(juce::Rectangle<int> area, juce::Component& a, juce::Component& b, juce::Component& c, juce::Component& d)
{
    const int w = area.getWidth() / 4;
    a.setBounds(area.removeFromLeft(w).reduced(4));
    b.setBounds(area.removeFromLeft(w).reduced(4));
    c.setBounds(area.removeFromLeft(w).reduced(4));
    d.setBounds(area.reduced(4));
}
} // namespace

void MoogVoyagerAudioProcessorEditor::styleKnob(juce::Slider& s)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 56, 16);
}

void MoogVoyagerAudioProcessorEditor::bindCombo(juce::ComboBox& box, const juce::StringArray& items)
{
    box.addItemList(items, 1);
}

MoogVoyagerAudioProcessorEditor::MoogVoyagerAudioProcessorEditor(MoogVoyagerAudioProcessor& p)
    : AudioProcessorEditor(&p),
      processorRef(p)
{
    styleKnob(masterSlider);
    addAndMakeVisible(masterSlider);

    const juce::StringArray waves { "Saw", "Square", "Triangle", "Pulse" };
    const juce::StringArray octaves { "-2", "-1", "0", "+1", "+2" };

    for (int i = 0; i < 3; ++i)
    {
        styleKnob(oscLevel[i]);
        styleKnob(oscFine[i]);
        bindCombo(oscOctave[i], octaves);
        bindCombo(oscWave[i], waves);
        addAndMakeVisible(oscLevel[i]);
        addAndMakeVisible(oscFine[i]);
        addAndMakeVisible(oscOctave[i]);
        addAndMakeVisible(oscWave[i]);
    }

    styleKnob(noiseSlider);
    addAndMakeVisible(noiseSlider);

    for (auto* s : { &filterCutoff, &filterRes, &filterDrive, &filterEnvAmt, &filterKb,
                     &fAttack, &fDecay, &fSustain, &fRelease, &aAttack, &aDecay, &aSustain, &aRelease,
                     &lfoRate, &lfoPitch, &lfoFilter, &glide })
    {
        styleKnob(*s);
        addAndMakeVisible(*s);
    }

    auto& apvts = processorRef.getApvts();
    masterAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::outputGainDb), masterSlider);

    const std::string_view levelIds[3] = { SynthParamIDs::osc1Level, SynthParamIDs::osc2Level, SynthParamIDs::osc3Level };
    const std::string_view fineIds[3] = { SynthParamIDs::osc1Fine, SynthParamIDs::osc2Fine, SynthParamIDs::osc3Fine };
    const std::string_view octIds[3] = { SynthParamIDs::osc1Octave, SynthParamIDs::osc2Octave, SynthParamIDs::osc3Octave };
    const std::string_view waveIds[3] = { SynthParamIDs::osc1Wave, SynthParamIDs::osc2Wave, SynthParamIDs::osc3Wave };

    for (int i = 0; i < 3; ++i)
    {
        oscLevelAttachments[i] = std::make_unique<SliderAttachment>(apvts, std::string(levelIds[i]), oscLevel[i]);
        oscFineAttachments[i] = std::make_unique<SliderAttachment>(apvts, std::string(fineIds[i]), oscFine[i]);
        oscOctaveAttachments[i] = std::make_unique<ComboAttachment>(apvts, std::string(octIds[i]), oscOctave[i]);
        oscWaveAttachments[i] = std::make_unique<ComboAttachment>(apvts, std::string(waveIds[i]), oscWave[i]);
    }

    noiseAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::noiseLevel), noiseSlider);
    filterCutoffAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::filterCutoff), filterCutoff);
    filterResAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::filterResonance), filterRes);
    filterDriveAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::filterDrive), filterDrive);
    filterEnvAmtAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::filterEnvAmount), filterEnvAmt);
    filterKbAttachment = std::make_unique<SliderAttachment>(apvts, std::string(SynthParamIDs::filterKeyTrack), filterKb);

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

    setSize(980, 520);
}

MoogVoyagerAudioProcessorEditor::~MoogVoyagerAudioProcessorEditor() = default;

void MoogVoyagerAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(kPanelBlue);
    g.setColour(kPanelWood);
    g.fillRect(getLocalBounds().removeFromTop(48));
    g.setColour(kAccent);
    g.setFont(juce::FontOptions(22.0f, juce::Font::bold));
    g.drawFittedText("V Voyager — Minimoog Voyager-style mono synth", getLocalBounds().removeFromTop(48),
                     juce::Justification::centred, 1);

    g.setColour(juce::Colours::white.withAlpha(0.85f));
    g.setFont(13.0f);
    auto b = getLocalBounds().reduced(8).withTrimmedTop(52);
    g.drawText("Oscillators", b.removeFromTop(18).removeFromLeft(320), juce::Justification::centredLeft);
    g.drawText("Filter / LFO", b.removeFromTop(0), juce::Justification::centred);
}

void MoogVoyagerAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(10).withTrimmedTop(56);
    auto top = area.removeFromTop(200);
    auto oscRow = top.removeFromTop(180);

    for (int i = 0; i < 3; ++i)
    {
        auto col = oscRow.removeFromLeft(oscRow.getWidth() / 3).reduced(6);
        auto r1 = col.removeFromTop(col.getHeight() / 2);
        oscLevel[i].setBounds(r1.removeFromLeft(r1.getWidth() / 2).reduced(2));
        oscFine[i].setBounds(r1.reduced(2));
        oscOctave[i].setBounds(col.removeFromTop(28));
        oscWave[i].setBounds(col.removeFromTop(28));
    }

    auto mid = area.removeFromTop(140);
    auto filterArea = mid.removeFromLeft(mid.getWidth() * 2 / 3);
    layoutRow(filterArea.removeFromTop(70), filterCutoff, filterRes, filterDrive, filterEnvAmt);
    filterKb.setBounds(filterArea.reduced(8));

    auto envArea = mid;
    layoutRow(envArea.removeFromTop(65), fAttack, fDecay, fSustain, fRelease);
    layoutRow(envArea, aAttack, aDecay, aSustain, aRelease);

    auto bottom = area;
    layoutRow(bottom.removeFromTop(80), lfoRate, lfoPitch, lfoFilter, glide);
    auto tail = bottom;
    noiseSlider.setBounds(tail.removeFromLeft(120).reduced(8));
    masterSlider.setBounds(tail.removeFromRight(140).reduced(8));
}
