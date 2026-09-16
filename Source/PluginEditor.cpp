#include "PluginEditor.h"

#include "Assets/FactoryPatchLibrary.h"
#include "Assets/RomFormat.h"
#include "Parameters/EnvelopeParameters.h"
#include "Assets/WavePalette.h"

JDUpgradedAudioProcessorEditor::JDUpgradedAudioProcessorEditor (JDUpgradedAudioProcessor& p)
    : AudioProcessorEditor (&p), processor_ (p)
{
    titleLabel_.setText ("JD Upgraded", juce::dontSendNotification);
    titleLabel_.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (titleLabel_);

    romSourceLabel_.setJustificationType (juce::Justification::centred);
    romSourceLabel_.setFont (juce::Font (11.0f));
    addAndMakeVisible (romSourceLabel_);

    programLabel_.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (programLabel_);

    programPrev_.onClick = [this] { changeProgramByDelta (-1); };
    programNext_.onClick = [this] { changeProgramByDelta (1); };
    addAndMakeVisible (programPrev_);
    addAndMakeVisible (programNext_);

    styleRotary (masterGainSlider_, "Master");
    addAndMakeVisible (masterGainSlider_);
    masterGainSlider_.onValueChange = [this]
    {
        if (auto* param = processor_.getAPVTS().getParameter ("masterGain"))
            param->setValueNotifyingHost (static_cast<float> (masterGainSlider_.getValue()));
    };
    if (auto* raw = processor_.getAPVTS().getRawParameterValue ("masterGain"))
        masterGainSlider_.setValue (raw->load(), juce::dontSendNotification);

    styleRotary (filterResonanceSlider_, "Res");
    styleRotary (groupADriveSlider_, "Grp A");
    styleRotary (groupBMixSlider_, "Grp B");
    addAndMakeVisible (filterResonanceSlider_);
    addAndMakeVisible (groupADriveSlider_);
    addAndMakeVisible (groupBMixSlider_);

    filterAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor_.getAPVTS(), "filterResonance", filterResonanceSlider_);
    groupAAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor_.getAPVTS(), "groupADrive", groupADriveSlider_);
    groupBAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor_.getAPVTS(), "groupBMix", groupBMixSlider_);

    couplingCombo_.addItemList ({ "Independent", "Ring 1-2", "Ring 3-4", "X-Mod 1-2", "X-Mod 3-4", "Sync 1-2", "Sync 3-4" }, 1);
    addAndMakeVisible (couplingCombo_);
    couplingAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        processor_.getAPVTS(), "couplingMode", couplingCombo_);

    const char* levelIds[4] = { "tone1Level", "tone2Level", "tone3Level", "tone4Level" };
    const char* waveIds[4] = { "tone1Wave", "tone2Wave", "tone3Wave", "tone4Wave" };
    const char* muteIds[4] = { "tone1Mute", "tone2Mute", "tone3Mute", "tone4Mute" };
    const char* msIds[4] = { "tone1Multisample", "tone2Multisample", "tone3Multisample", "tone4Multisample" };
    const int maxWave = static_cast<int> (jdupgraded::assets::kCleanroomWaveCount) - 1;
    const int maxMs = static_cast<int> (jdupgraded::assets::kMultisampleSetCount);

    for (int i = 0; i < 4; ++i)
    {
        auto& level = toneLevelSliders_[static_cast<std::size_t> (i)];
        level.setSliderStyle (juce::Slider::LinearVertical);
        level.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 44, 14);
        addAndMakeVisible (level);
        toneLevelAttachments_[static_cast<std::size_t> (i)] =
            std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                processor_.getAPVTS(), levelIds[i], level);

        auto& wave = toneWaveSliders_[static_cast<std::size_t> (i)];
        wave.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        wave.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 44, 14);
        wave.setRange (0, maxWave, 1.0);
        addAndMakeVisible (wave);
        toneWaveAttachments_[static_cast<std::size_t> (i)] =
            std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                processor_.getAPVTS(), waveIds[i], wave);

        auto& ms = toneMultisampleSliders_[static_cast<std::size_t> (i)];
        ms.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        ms.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 44, 14);
        ms.setRange (0, maxMs, 1.0);
        ms.setName ("MS" + juce::String (i + 1));
        addAndMakeVisible (ms);
        toneMsAttachments_[static_cast<std::size_t> (i)] =
            std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                processor_.getAPVTS(), msIds[i], ms);

        toneMuteButtons_[static_cast<std::size_t> (i)].setButtonText ("M" + juce::String (i + 1));
        toneMuteButtons_[static_cast<std::size_t> (i)].setClickingTogglesState (true);
        addAndMakeVisible (toneMuteButtons_[static_cast<std::size_t> (i)]);
        toneMuteAttachments_[static_cast<std::size_t> (i)] =
            std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
                processor_.getAPVTS(), muteIds[i], toneMuteButtons_[static_cast<std::size_t> (i)]);

        auto& palette = tonePaletteCombos_[static_cast<std::size_t> (i)];
        palette.addItem ("Palette", 1);
        palette.addItem ("Analog", 2);
        palette.addItem ("Digital", 3);
        palette.addItem ("Bell", 4);
        palette.addItem ("Pluck", 5);
        palette.addItem ("Bass", 6);
        palette.addItem ("Noise", 7);
        palette.addItem ("Vocal", 8);
        palette.addItem ("Organ", 9);
        palette.addItem ("FX", 10);
        palette.setSelectedId (1, juce::dontSendNotification);
        palette.onChange = [this, i]
        {
            snapToneToPaletteCategory (i, tonePaletteCombos_[static_cast<std::size_t> (i)].getSelectedId());
        };
        addAndMakeVisible (palette);

        tonePaletteLabels_[static_cast<std::size_t> (i)].setJustificationType (juce::Justification::centred);
        addAndMakeVisible (tonePaletteLabels_[static_cast<std::size_t> (i)]);
    }

    ampEnvLabel_.setText ("Amp ADSR", juce::dontSendNotification);
    filterEnvLabel_.setText ("Filter ADSR", juce::dontSendNotification);
    addAndMakeVisible (ampEnvLabel_);
    addAndMakeVisible (filterEnvLabel_);

    styleEnvSlider (ampAttackSlider_, "A");
    styleEnvSlider (ampDecaySlider_, "D");
    styleEnvSlider (ampSustainSlider_, "S");
    styleEnvSlider (ampReleaseSlider_, "R");
    styleEnvSlider (filterAttackSlider_, "A");
    styleEnvSlider (filterDecaySlider_, "D");
    styleEnvSlider (filterSustainSlider_, "S");
    styleEnvSlider (filterReleaseSlider_, "R");

    for (auto* slider : { &ampAttackSlider_, &ampDecaySlider_, &ampSustainSlider_, &ampReleaseSlider_,
                          &filterAttackSlider_, &filterDecaySlider_, &filterSustainSlider_, &filterReleaseSlider_ })
        addAndMakeVisible (*slider);

    envelopeLinkButton_.setButtonText ("Link");
    envelopeLinkButton_.setClickingTogglesState (true);
    addAndMakeVisible (envelopeLinkButton_);
    envelopeLinkAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        processor_.getAPVTS(), jdupgraded::params::kEnvelopeLinkId, envelopeLinkButton_);
    envelopeLinkButton_.onClick = [this]
    {
        if (processor_.isEnvelopeLinked())
            processor_.copyGlobalEnvelopesToAllTones();
        envelopeToneCombo_.setEnabled (! processor_.isEnvelopeLinked());
        rebindEnvelopeAttachments();
    };

    envelopeToneLabel_.setText ("Tone", juce::dontSendNotification);
    addAndMakeVisible (envelopeToneLabel_);
    envelopeToneCombo_.addItemList ({ "1", "2", "3", "4" }, 1);
    envelopeToneCombo_.setSelectedId (1, juce::dontSendNotification);
    envelopeToneCombo_.setEnabled (! processor_.isEnvelopeLinked());
    envelopeToneCombo_.onChange = [this] { rebindEnvelopeAttachments(); };
    addAndMakeVisible (envelopeToneCombo_);

    rebindEnvelopeAttachments();

    startTimerHz (4);
    setSize (720, 600);
}

JDUpgradedAudioProcessorEditor::~JDUpgradedAudioProcessorEditor()
{
    stopTimer();
}

void JDUpgradedAudioProcessorEditor::styleRotary (juce::Slider& slider, const juce::String& name)
{
    slider.setName (name);
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 56, 14);
}

void JDUpgradedAudioProcessorEditor::styleEnvSlider (juce::Slider& slider, const juce::String& name)
{
    slider.setName (name);
    slider.setSliderStyle (juce::Slider::LinearHorizontal);
    slider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 48, 16);
}

void JDUpgradedAudioProcessorEditor::rebindEnvelopeAttachments()
{
    ampAttackAttachment_.reset();
    ampDecayAttachment_.reset();
    ampSustainAttachment_.reset();
    ampReleaseAttachment_.reset();
    filterAttackAttachment_.reset();
    filterDecayAttachment_.reset();
    filterSustainAttachment_.reset();
    filterReleaseAttachment_.reset();

    const bool linked = processor_.isEnvelopeLinked();
    const int tone = envelopeToneCombo_.getSelectedId() > 0 ? envelopeToneCombo_.getSelectedId() : 1;

    const juce::String ampAttackId =
        linked ? "ampAttack" : jdupgraded::params::toneEnvelopeParamId (tone, "AmpAttack");
    const juce::String ampDecayId =
        linked ? "ampDecay" : jdupgraded::params::toneEnvelopeParamId (tone, "AmpDecay");
    const juce::String ampSustainId =
        linked ? "ampSustain" : jdupgraded::params::toneEnvelopeParamId (tone, "AmpSustain");
    const juce::String ampReleaseId =
        linked ? "ampRelease" : jdupgraded::params::toneEnvelopeParamId (tone, "AmpRelease");
    const juce::String filterAttackId =
        linked ? "filterAttack" : jdupgraded::params::toneEnvelopeParamId (tone, "FilterAttack");
    const juce::String filterDecayId =
        linked ? "filterDecay" : jdupgraded::params::toneEnvelopeParamId (tone, "FilterDecay");
    const juce::String filterSustainId =
        linked ? "filterSustain" : jdupgraded::params::toneEnvelopeParamId (tone, "FilterSustain");
    const juce::String filterReleaseId =
        linked ? "filterRelease" : jdupgraded::params::toneEnvelopeParamId (tone, "FilterRelease");

    ampAttackAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor_.getAPVTS(), ampAttackId, ampAttackSlider_);
    ampDecayAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor_.getAPVTS(), ampDecayId, ampDecaySlider_);
    ampSustainAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor_.getAPVTS(), ampSustainId, ampSustainSlider_);
    ampReleaseAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor_.getAPVTS(), ampReleaseId, ampReleaseSlider_);
    filterAttackAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor_.getAPVTS(), filterAttackId, filterAttackSlider_);
    filterDecayAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor_.getAPVTS(), filterDecayId, filterDecaySlider_);
    filterSustainAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor_.getAPVTS(), filterSustainId, filterSustainSlider_);
    filterReleaseAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor_.getAPVTS(), filterReleaseId, filterReleaseSlider_);
}

void JDUpgradedAudioProcessorEditor::snapToneToPaletteCategory (int toneIndex0Based, int comboItemId)
{
    if (comboItemId < 2)
        return;

    const auto category = static_cast<jdupgraded::assets::RomWaveCategory> (comboItemId - 2);
    const auto& bank = processor_.getRomBank();
    std::size_t waveIndex = 0;
    if (bank.isLoaded())
    {
        waveIndex = bank.findFirstWaveInCategory (category);
    }
    else
    {
        for (std::size_t i = 0; i < jdupgraded::assets::kCleanroomWaveCount; ++i)
        {
            if (jdupgraded::assets::categoryForWaveIndexFallback (i) == category)
            {
                waveIndex = i;
                break;
            }
        }
    }

    const char* waveIds[4] = { "tone1Wave", "tone2Wave", "tone3Wave", "tone4Wave" };
    const char* msIds[4] = { "tone1Multisample", "tone2Multisample", "tone3Multisample", "tone4Multisample" };

    if (auto* waveParam = processor_.getAPVTS().getParameter (waveIds[toneIndex0Based]))
        waveParam->setValueNotifyingHost (waveParam->convertTo0to1 (static_cast<float> (waveIndex)));

    const std::uint16_t ms = bank.isLoaded() ? bank.getMultisampleSetId (waveIndex) : 0;
    if (auto* msParam = processor_.getAPVTS().getParameter (msIds[toneIndex0Based]))
        msParam->setValueNotifyingHost (msParam->convertTo0to1 (static_cast<float> (ms)));
}

juce::String JDUpgradedAudioProcessorEditor::describeWaveForTone (int toneIndex0Based) const
{
    const char* waveIds[4] = { "tone1Wave", "tone2Wave", "tone3Wave", "tone4Wave" };
    const auto& bank = processor_.getRomBank();
    int waveIndex = 0;
    if (auto* raw = processor_.getAPVTS().getRawParameterValue (waveIds[toneIndex0Based]))
        waveIndex = static_cast<int> (raw->load());

    const auto category = bank.isLoaded()
                              ? bank.getWaveCategory (static_cast<std::size_t> (waveIndex))
                              : jdupgraded::assets::categoryForWaveIndexFallback (static_cast<std::size_t> (waveIndex));

    juce::String text = jdupgraded::assets::categoryDisplayName (category);
    text << " #" << (waveIndex + 1);

    if (bank.isLoaded())
    {
        const auto ms = bank.getMultisampleSetId (static_cast<std::size_t> (waveIndex));
        if (ms > 0)
            text << " MS" << ms;
    }

    return text;
}

void JDUpgradedAudioProcessorEditor::changeProgramByDelta (int delta)
{
    const int count = static_cast<int> (jdupgraded::assets::FactoryPatchLibrary::getPatchCount());
    int program = processor_.getCurrentProgram() + delta;
    program = juce::jlimit (0, count - 1, program);
    processor_.setCurrentProgram (program);
}

void JDUpgradedAudioProcessorEditor::timerCallback()
{
    const int program = processor_.getCurrentProgram();
    const auto& patch = jdupgraded::assets::FactoryPatchLibrary::getPatch (static_cast<std::size_t> (program));
    programLabel_.setText (juce::String (program + 1) + " / "
                           + juce::String (jdupgraded::assets::FactoryPatchLibrary::getPatchCount())
                           + "  " + patch.name,
                           juce::dontSendNotification);

    for (int i = 0; i < 4; ++i)
        tonePaletteLabels_[static_cast<std::size_t> (i)].setText (describeWaveForTone (i),
                                                                  juce::dontSendNotification);

    romSourceLabel_.setText (processor_.getRomSourceDescription(), juce::dontSendNotification);
}

void JDUpgradedAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1a1a22));
    g.setColour (juce::Colours::white.withAlpha (0.85f));
    g.drawFittedText ("Palette snap per tone  |  Envelopes: Link or pick tone 1–4",
                      getLocalBounds().removeFromBottom (24),
                      juce::Justification::centred, 1);

    const int colW = getWidth() / 4;
    for (int i = 0; i < 4; ++i)
    {
        const char* waveIds[4] = { "tone1Wave", "tone2Wave", "tone3Wave", "tone4Wave" };
        int waveIndex = 0;
        if (auto* raw = processor_.getAPVTS().getRawParameterValue (waveIds[i]))
            waveIndex = static_cast<int> (raw->load());

        const auto& bank = processor_.getRomBank();
        const auto category = bank.isLoaded()
                                  ? bank.getWaveCategory (static_cast<std::size_t> (waveIndex))
                                  : jdupgraded::assets::categoryForWaveIndexFallback (static_cast<std::size_t> (waveIndex));

        auto stripe = juce::Rectangle<int> (i * colW + 12, 28, colW - 24, 4);
        g.setColour (jdupgraded::assets::categoryAccentColour (category));
        g.fillRect (stripe);
    }
}

void JDUpgradedAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (8);
    titleLabel_.setBounds (area.removeFromTop (18));
    romSourceLabel_.setBounds (area.removeFromTop (14));

    auto programRow = area.removeFromTop (26);
    programPrev_.setBounds (programRow.removeFromLeft (32));
    programNext_.setBounds (programRow.removeFromRight (32));
    programLabel_.setBounds (programRow);

    auto fxRow = area.removeFromTop (88);
    const int fxW = fxRow.getWidth() / 5;
    masterGainSlider_.setBounds (fxRow.removeFromLeft (fxW).reduced (4));
    filterResonanceSlider_.setBounds (fxRow.removeFromLeft (fxW).reduced (4));
    groupADriveSlider_.setBounds (fxRow.removeFromLeft (fxW).reduced (4));
    groupBMixSlider_.setBounds (fxRow.removeFromLeft (fxW).reduced (4));
    couplingCombo_.setBounds (fxRow.reduced (4));

    auto envArea = area.removeFromBottom (108);
    area = area.reduced (0, 4);

    auto toneRow = area;
    const int colW = toneRow.getWidth() / 4;
    for (int i = 0; i < 4; ++i)
    {
        auto col = toneRow.removeFromLeft (colW).reduced (4);
        tonePaletteLabels_[static_cast<std::size_t> (i)].setBounds (col.removeFromBottom (16));
        tonePaletteCombos_[static_cast<std::size_t> (i)].setBounds (col.removeFromBottom (22));
        toneMuteButtons_[static_cast<std::size_t> (i)].setBounds (col.removeFromBottom (22));
        auto topHalf = col.removeFromTop (col.getHeight() / 2);
        toneWaveSliders_[static_cast<std::size_t> (i)].setBounds (topHalf.removeFromLeft (topHalf.getWidth() / 2).reduced (2));
        toneMultisampleSliders_[static_cast<std::size_t> (i)].setBounds (topHalf.reduced (2));
        toneLevelSliders_[static_cast<std::size_t> (i)].setBounds (col.reduced (2));
    }

    auto envHeader = envArea.removeFromTop (22);
    envelopeLinkButton_.setBounds (envHeader.removeFromLeft (52));
    envelopeToneLabel_.setBounds (envHeader.removeFromLeft (40));
    envelopeToneCombo_.setBounds (envHeader.removeFromLeft (48));

    auto ampRow = envArea.removeFromTop (40);
    ampEnvLabel_.setBounds (ampRow.removeFromLeft (72));
    const int envSlot = ampRow.getWidth() / 4;
    ampAttackSlider_.setBounds (ampRow.removeFromLeft (envSlot).reduced (2));
    ampDecaySlider_.setBounds (ampRow.removeFromLeft (envSlot).reduced (2));
    ampSustainSlider_.setBounds (ampRow.removeFromLeft (envSlot).reduced (2));
    ampReleaseSlider_.setBounds (ampRow.reduced (2));

    auto fltRow = envArea;
    filterEnvLabel_.setBounds (fltRow.removeFromLeft (72));
    const int fltSlot = fltRow.getWidth() / 4;
    filterAttackSlider_.setBounds (fltRow.removeFromLeft (fltSlot).reduced (2));
    filterDecaySlider_.setBounds (fltRow.removeFromLeft (fltSlot).reduced (2));
    filterSustainSlider_.setBounds (fltRow.removeFromLeft (fltSlot).reduced (2));
    filterReleaseSlider_.setBounds (fltRow.reduced (2));
}
