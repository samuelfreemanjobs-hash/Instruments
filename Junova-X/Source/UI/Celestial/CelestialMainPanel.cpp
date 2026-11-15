#include "CelestialMainPanel.h"

#include "PluginProcessor.h"
#include "UI/JunovaLookAndFeel.h"

namespace junovax::ui::celestial
{
ModuleShell::ModuleShell (juce::String title, juce::Colour accent, juce::Colour fill)
    : title_ (std::move (title)), accent_ (accent), fill_ (fill)
{
    titleLabel_.setText (title_, juce::dontSendNotification);
    titleLabel_.setJustificationType (juce::Justification::centredLeft);
    titleLabel_.setColour (juce::Label::textColourId, Theme::textPrimary());
    titleLabel_.setFont (juce::FontOptions (12.0f).withStyle ("Bold"));
    addAndMakeVisible (titleLabel_);
}

void ModuleShell::paint (juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();
    g.setColour (fill_);
    g.fillRoundedRectangle (b, 8.0f);
    g.setColour (accent_.withAlpha (0.55f));
    g.drawRoundedRectangle (b.reduced (0.5f), 8.0f, 1.5f);
}

void ModuleShell::resized()
{
    titleLabel_.setBounds (10, 6, getWidth() - 20, 18);
}

juce::Rectangle<int> ModuleShell::getContentArea() const
{
    return getLocalBounds().reduced (8).withTrimmedTop (26);
}

CelestialMainPanel::CelestialMainPanel (JunovaXAudioProcessor& processor)
    : processor_ (processor), oscMonitor_ (processor)
{
    setLookAndFeel (&laf_);
    laf_.setColour (juce::Slider::trackColourId, Theme::glowBlue().withAlpha (0.3f));
    laf_.setColour (juce::Slider::thumbColourId, Theme::textPrimary());
    laf_.setColour (juce::Slider::backgroundColourId, juce::Colour (0xff0c0c14));

    brand_.setFont (juce::FontOptions (26.0f).withStyle ("Bold"));
    brand_.setColour (juce::Label::textColourId, Theme::textPrimary());
    tagline_.setColour (juce::Label::textColourId, Theme::textMuted());
    hostBar_.setColour (juce::Label::textColourId, Theme::textMuted());
    hostBar_.setFont (juce::FontOptions (11.0f));
    presetLabel_.setColour (juce::Label::textColourId, Theme::glowCyan());
    presetLabel_.setFont (juce::FontOptions (13.0f).withStyle ("Bold"));

    for (auto* b : { &voicePoly1_, &voicePoly2_, &voiceUnison_, &voiceMono_, &voiceJuno6_ })
    {
        b->setClickingTogglesState (false);
        addAndMakeVisible (*b);
    }
    constexpr float kVoiceSteps = 4.0f;
    voicePoly1_.onClick = [this] { if (auto* p = processor_.getApvts().getParameter (junovax::ParameterIDs::voiceMode)) p->setValueNotifyingHost (0.0f); };
    voicePoly2_.onClick = [this] { if (auto* p = processor_.getApvts().getParameter (junovax::ParameterIDs::voiceMode)) p->setValueNotifyingHost (1.0f / kVoiceSteps); };
    voiceUnison_.onClick = [this] { if (auto* p = processor_.getApvts().getParameter (junovax::ParameterIDs::voiceMode)) p->setValueNotifyingHost (2.0f / kVoiceSteps); };
    voiceMono_.onClick = [this] { if (auto* p = processor_.getApvts().getParameter (junovax::ParameterIDs::voiceMode)) p->setValueNotifyingHost (3.0f / kVoiceSteps); };
    voiceJuno6_.onClick = [this] { if (auto* p = processor_.getApvts().getParameter (junovax::ParameterIDs::voiceMode)) p->setValueNotifyingHost (1.0f); };

    addAndMakeVisible (brand_);
    addAndMakeVisible (tagline_);
    addAndMakeVisible (hostBar_);
    addAndMakeVisible (presetLabel_);
    addAndMakeVisible (presetPrev_);
    addAndMakeVisible (presetNext_);
    for (auto* b : { &saveBtn_, &initBtn_, &vaultBtn_, &abBtn_ })
        addAndMakeVisible (*b);
    saveBtn_.onClick = [this] { processor_.saveUserPresetToFile(); };
    initBtn_.onClick = [this] { processor_.initToDefaultPatch(); refreshPresetLabel(); };
    vaultBtn_.onClick = [this] { if (onOpenVault_) onOpenVault_(); };
    abBtn_.onClick = [this] { processor_.toggleAbSlot(); };
    presetPrev_.onClick = [this]
    {
        const int n = processor_.getNumPrograms();
        if (n <= 0)
            return;
        int idx = processor_.getCurrentProgram() - 1;
        if (idx < 0)
            idx = n - 1;
        processor_.setCurrentProgram (idx);
        refreshPresetLabel();
    };
    presetNext_.onClick = [this]
    {
        const int n = processor_.getNumPrograms();
        if (n <= 0)
            return;
        int idx = (processor_.getCurrentProgram() + 1) % n;
        processor_.setCurrentProgram (idx);
        refreshPresetLabel();
    };
    refreshPresetLabel();
    refreshHostBar();
    startTimerHz (4);

    addAndMakeVisible (oscMonitor_);

    for (auto* m : { &lfoModule_, &dcoModule_, &hpfModule_, &vcfModule_, &envModule_,
                     &chorusModule_, &driftModule_, &arpModule_, &masterModule_ })
        addAndMakeVisible (*m);

    auto& lfo = lfoModule_;
    addVerticalFader (lfo, junovax::ParameterIDs::lfoRate, "RATE");
    addVerticalFader (lfo, junovax::ParameterIDs::lfoDelay, "DELAY");
    addVerticalFader (lfo, junovax::ParameterIDs::glide, "GLIDE");

    auto& dco = dcoModule_;
    addVerticalFader (dco, junovax::ParameterIDs::dcoLfoMod, "LFO MOD");
    addVerticalFader (dco, junovax::ParameterIDs::dcoPwm, "PWM");
    addVerticalFader (dco, junovax::ParameterIDs::dcoSubLvl, "SUB");
    addVerticalFader (dco, junovax::ParameterIDs::dcoNoise, "NOISE");

    for (auto* b : { &hpfOff_, &hpf1_, &hpf2_, &hpf3_ })
        hpfModule_.addAndMakeVisible (*b);
    auto setHpf = [this] (float norm)
    {
        if (auto* p = processor_.getApvts().getParameter (junovax::ParameterIDs::hpfMode))
            p->setValueNotifyingHost (norm);
    };
    hpfOff_.onClick = [setHpf] { setHpf (0.0f); };
    hpf1_.onClick = [setHpf] { setHpf (0.33f); };
    hpf2_.onClick = [setHpf] { setHpf (0.66f); };
    hpf3_.onClick = [setHpf] { setHpf (1.0f); };

    auto& vcf = vcfModule_;
    addVerticalFader (vcf, junovax::ParameterIDs::filterCutoff, "FREQ");
    addVerticalFader (vcf, junovax::ParameterIDs::filterRes, "RES");
    addVerticalFader (vcf, junovax::ParameterIDs::vcfEnv, "ENV");
    addVerticalFader (vcf, junovax::ParameterIDs::vcfLfo, "LFO");
    addVerticalFader (vcf, junovax::ParameterIDs::vcfKey, "KYBD");

    auto& env = envModule_;
    addVerticalFader (env, junovax::ParameterIDs::ampAttack, "A");
    addVerticalFader (env, junovax::ParameterIDs::ampDecay, "D");
    addVerticalFader (env, junovax::ParameterIDs::ampSustain, "S");
    addVerticalFader (env, junovax::ParameterIDs::ampRelease, "R");

    addChorusModeButtons (chorusModule_);

    addVerticalFader (driftModule_, junovax::ParameterIDs::drift, "DRIFT");
    addVerticalFader (driftModule_, junovax::ParameterIDs::detune, "DETUNE");
    addVerticalFader (driftModule_, junovax::ParameterIDs::width, "WIDTH");

    addVerticalFader (arpModule_, junovax::ParameterIDs::arpRange, "RANGE");
    addVerticalFader (arpModule_, junovax::ParameterIDs::arpRate, "RATE");
    arpModule_.addAndMakeVisible (arpLatch_);
    arpLatchAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), junovax::ParameterIDs::arpLatch, arpLatch_);

    addVerticalFader (masterModule_, junovax::ParameterIDs::masterGain, "MASTER");
    masterModule_.addAndMakeVisible (peakMeter_);
}

CelestialMainPanel::~CelestialMainPanel()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void CelestialMainPanel::timerCallback()
{
    refreshPresetLabel();
    refreshHostBar();
    peakLevel_ = processor_.getOutputPeak();
    peakMeter_.repaint();
}

void CelestialMainPanel::refreshHostBar()
{
    const float bpm = processor_.getDisplayBpm();
    const int sr = static_cast<int> (processor_.getEffectiveSampleRate() + 0.5);
    hostBar_.setText ("BPM: " + juce::String (bpm, 2) + "  |  4/4  |  SR: " + juce::String (sr) + " Hz",
                      juce::dontSendNotification);
}

void CelestialMainPanel::refreshPresetLabel()
{
    const int idx = processor_.getCurrentProgram();
    const auto name = processor_.getProgramName (idx);
    presetLabel_.setText (juce::String (idx + 1) + " / " + juce::String (processor_.getNumPrograms()) + "  " + name,
                          juce::dontSendNotification);
}

juce::Slider& CelestialMainPanel::addVerticalFader (juce::Component& parent, const char* paramId, juce::String name)
{
    auto slider = std::make_unique<juce::Slider> (juce::Slider::LinearVertical, juce::Slider::TextBoxBelow);
    slider->setName (name);
    slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 52, 14);
    const bool vcf = juce::String (paramId).containsIgnoreCase ("filter")
                     || juce::String (paramId).containsIgnoreCase ("vcf")
                     || juce::String (paramId).containsIgnoreCase ("amp");
    junovax::ui::JunovaLookAndFeel::tagSlider (*slider, vcf ? junovax::ui::JunovaLookAndFeel::Accent::crimson
                                                             : junovax::ui::JunovaLookAndFeel::Accent::cyan);
    parent.addAndMakeVisible (*slider);
    sliderAttachments_.push_back (std::make_unique<SliderAttachment> (processor_.getApvts(), paramId, *slider));
    auto& ref = *slider;
    sliders_.push_back (std::move (slider));
    return ref;
}

void CelestialMainPanel::addChorusModeButtons (juce::Component& parent)
{
    for (auto* b : { &chorusOff_, &chorusI_, &chorusII_, &chorusBoth_ })
    {
        b->setClickingTogglesState (false);
        parent.addAndMakeVisible (*b);
    }
    auto setMode = [this] (float norm)
    {
        if (auto* p = processor_.getApvts().getParameter (junovax::ParameterIDs::chorusMode))
            p->setValueNotifyingHost (norm);
    };
    chorusOff_.onClick = [setMode] { setMode (0.0f); };
    chorusI_.onClick = [setMode] { setMode (0.33f); };
    chorusII_.onClick = [setMode] { setMode (0.66f); };
    chorusBoth_.onClick = [setMode] { setMode (1.0f); };
}

void CelestialMainPanel::paint (juce::Graphics& g)
{
    g.fillAll (Theme::background());
    g.setColour (Theme::panelGrey());
    g.fillRect (0, 0, getWidth(), 28);
    g.setColour (Theme::panelBlue().darker (0.4f));
    g.fillRect (0, 28, getWidth(), 36);

    auto meter = peakMeter_.getBounds();
    if (! meter.isEmpty())
    {
        g.setColour (Theme::ledInactive());
        g.fillRoundedRectangle (meter.toFloat(), 4.0f);
        const float h = meter.getHeight() * juce::jlimit (0.0f, 1.0f, peakLevel_ * 4.0f);
        g.setColour (Theme::electricCyan());
        g.fillRoundedRectangle (meter.getX() + 6.0f,
                                meter.getBottom() - h,
                                static_cast<float> (meter.getWidth()) - 12.0f,
                                h,
                                3.0f);
    }
}

void CelestialMainPanel::resized()
{
    using L = junovax::ui::Layout;
    const int w = getWidth();
    const int h = getHeight();

    hostBar_.setBounds (8, 4, w - 16, 20);
    brand_.setBounds (16, 34, 220, 30);
    tagline_.setBounds (16, 58, 280, 18);
    presetPrev_.setBounds (w / 2 - 168, 34, 28, 24);
    presetLabel_.setBounds (w / 2 - 136, 36, 272, 22);
    presetNext_.setBounds (w / 2 + 140, 34, 28, 24);
    saveBtn_.setBounds (w / 2 + 176, 34, 48, 24);
    initBtn_.setBounds (w / 2 + 228, 34, 44, 24);
    vaultBtn_.setBounds (w / 2 + 276, 34, 54, 24);
    abBtn_.setBounds (w / 2 + 334, 34, 40, 24);

    oscMonitor_.setBounds (w / 2 - 180, 64, 360, 88);

    const int voiceX = w - 220;
    voicePoly1_.setBounds (voiceX, 38, 48, 22);
    voicePoly2_.setBounds (voiceX + 52, 38, 48, 22);
    voiceUnison_.setBounds (voiceX + 104, 38, 56, 22);
    voiceMono_.setBounds (voiceX + 164, 38, 48, 22);
    voiceJuno6_.setBounds (voiceX + 104, 62, 56, 22);

    const int rowY = 160;
    const int rowH = 200;
    const int gap = 8;
    const int colW = (w - gap * 6) / 5;

    lfoModule_.setBounds (gap, rowY, colW, rowH);
    dcoModule_.setBounds (gap * 2 + colW, rowY, colW + 20, rowH);
    hpfModule_.setBounds (gap * 3 + colW * 2 + 20, rowY, colW - 40, rowH);
    vcfModule_.setBounds (gap * 4 + colW * 3 - 20, rowY, colW + 10, rowH);
    envModule_.setBounds (gap * 5 + colW * 4 - 10, rowY, colW, rowH);

    int idx = 0;
    auto placeRow = [&] (ModuleShell& shell, int count)
    {
        auto area = shell.getContentArea();
        const int fw = juce::jmax (32, area.getWidth() / count - 2);
        int x = area.getX() + 4;
        for (int i = 0; i < count && idx < static_cast<int> (sliders_.size()); ++i, ++idx)
        {
            sliders_[static_cast<std::size_t> (idx)]->setBounds (x, area.getY(), fw, area.getHeight());
            x += fw + 2;
        }
    };

    idx = 0;
    placeRow (lfoModule_, 3);
    placeRow (dcoModule_, 4);
    placeRow (hpfModule_, 0);
    {
        auto area = hpfModule_.getContentArea();
        const int bw = (area.getWidth() - 8) / 4;
        hpfOff_.setBounds (area.getX(), area.getY(), bw, 22);
        hpf1_.setBounds (area.getX() + bw + 2, area.getY(), bw, 22);
        hpf2_.setBounds (area.getX() + (bw + 2) * 2, area.getY(), bw, 22);
        hpf3_.setBounds (area.getX() + (bw + 2) * 3, area.getY(), bw, 22);
    }
    placeRow (vcfModule_, 5);
    placeRow (envModule_, 4);

    const int row2Y = rowY + rowH + gap;
    const int row2H = 120;
    const int w4 = (w - gap * 5) / 4;
    chorusModule_.setBounds (gap, row2Y, w4 + 40, row2H);
    driftModule_.setBounds (gap * 2 + w4 + 40, row2Y, w4 - 10, row2H);
    arpModule_.setBounds (gap * 3 + w4 * 2 + 30, row2Y, w4 - 10, row2H);
    masterModule_.setBounds (gap * 4 + w4 * 3 + 20, row2Y, w4 + 20, row2H);

    placeRow (driftModule_, 3);
    const int arpSliderIdx = idx;
    placeRow (arpModule_, 2);
    {
        auto area = arpModule_.getContentArea();
        arpLatch_.setBounds (area.getX() + 2, area.getY(), area.getWidth() - 4, 22);
        if (arpSliderIdx + 1 < static_cast<int> (sliders_.size()))
        {
            auto slot = area.withTrimmedTop (26);
            const int fw = juce::jmax (32, slot.getWidth() / 2 - 2);
            sliders_[static_cast<std::size_t> (arpSliderIdx)]->setBounds (slot.getX() + 4, slot.getY(), fw, slot.getHeight());
            sliders_[static_cast<std::size_t> (arpSliderIdx + 1)]->setBounds (slot.getX() + 6 + fw, slot.getY(), fw, slot.getHeight());
        }
    }
    placeRow (masterModule_, 1);
    {
        auto area = masterModule_.getContentArea();
        peakMeter_.setBounds (area.getRight() - 22, area.getY() + 8, 12, area.getHeight() - 16);
    }

    auto chArea = chorusModule_.getContentArea();
    const int bw = (chArea.getWidth() - 12) / 4;
    chorusOff_.setBounds (chArea.getX(), chArea.getY(), bw, 24);
    chorusI_.setBounds (chArea.getX() + bw + 4, chArea.getY(), bw, 24);
    chorusII_.setBounds (chArea.getX() + (bw + 4) * 2, chArea.getY(), bw, 24);
    chorusBoth_.setBounds (chArea.getX() + (bw + 4) * 3, chArea.getY(), bw, 24);

}

} // namespace junovax::ui::celestial
