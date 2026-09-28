#include "CelestialMainPanel.h"

#include "PluginProcessor.h"

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

    for (auto* b : { &voicePoly1_, &voicePoly2_, &voiceUnison_, &voiceMono_ })
    {
        b->setClickingTogglesState (false);
        addAndMakeVisible (*b);
    }
    voicePoly1_.onClick = [this] { if (auto* p = processor_.getApvts().getParameter (junovax::ParameterIDs::voiceMode)) p->setValueNotifyingHost (0.0f); };
    voicePoly2_.onClick = [this] { if (auto* p = processor_.getApvts().getParameter (junovax::ParameterIDs::voiceMode)) p->setValueNotifyingHost (1.0f / 3.0f); };
    voiceUnison_.onClick = [this] { if (auto* p = processor_.getApvts().getParameter (junovax::ParameterIDs::voiceMode)) p->setValueNotifyingHost (2.0f / 3.0f); };
    voiceMono_.onClick = [this] { if (auto* p = processor_.getApvts().getParameter (junovax::ParameterIDs::voiceMode)) p->setValueNotifyingHost (1.0f); };

    addAndMakeVisible (brand_);
    addAndMakeVisible (tagline_);
    addAndMakeVisible (hostBar_);
    addAndMakeVisible (presetLabel_);
    addAndMakeVisible (presetPrev_);
    addAndMakeVisible (presetNext_);
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

    addVerticalFader (hpfModule_, junovax::ParameterIDs::hpfCutoff, "HPF");
    hpfModule_.addAndMakeVisible (hpfEnable_);
    hpfAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), junovax::ParameterIDs::hpfEnabled, hpfEnable_);

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

    keyboardStrip_.setInterceptsMouseClicks (false, false);
    keyboardStrip_.setOpaque (false);
    addAndMakeVisible (keyboardStrip_);
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
}

void CelestialMainPanel::refreshHostBar()
{
    const float bpm = processor_.getDisplayBpm();
    const int sr = static_cast<int> (processor_.getEffectiveSampleRate() + 0.5);
    hostBar_.setText ("HOST | BPM: " + juce::String (bpm, 2) + " | 4/4 | SR: " + juce::String (sr) + " Hz",
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

    auto keyArea = keyboardStrip_.getBounds();
    if (! keyArea.isEmpty())
    {
        g.setColour (juce::Colour (0xff101018));
        g.fillRoundedRectangle (keyArea.toFloat(), 6.0f);
        const int keys = 24;
        const int kw = juce::jmax (8, keyArea.getWidth() / keys);
        for (int i = 0; i < keys; ++i)
        {
            const bool black = (i % 12 == 1 || i % 12 == 3 || i % 12 == 6 || i % 12 == 8 || i % 12 == 10);
            g.setColour (black ? juce::Colour (0xff050508) : juce::Colour (0xffececf4));
            g.fillRect (keyArea.getX() + i * kw, keyArea.getY(), kw - 1, keyArea.getHeight());
        }
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

    oscMonitor_.setBounds (w / 2 - 180, 64, 360, 88);

    const int voiceX = w - 220;
    voicePoly1_.setBounds (voiceX, 38, 48, 22);
    voicePoly2_.setBounds (voiceX + 52, 38, 48, 22);
    voiceUnison_.setBounds (voiceX + 104, 38, 56, 22);
    voiceMono_.setBounds (voiceX + 164, 38, 48, 22);

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
    const int hpfSliderIdx = idx;
    placeRow (hpfModule_, 1);
    {
        auto area = hpfModule_.getContentArea();
        hpfEnable_.setBounds (area.getX() + 2, area.getY(), area.getWidth() - 4, 22);
        if (hpfSliderIdx < static_cast<int> (sliders_.size()))
        {
            auto slot = area.withTrimmedTop (26);
            sliders_[static_cast<std::size_t> (hpfSliderIdx)]->setBounds (slot.getX() + 4, slot.getY(), slot.getWidth() - 8, slot.getHeight());
        }
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

    auto chArea = chorusModule_.getContentArea();
    const int bw = (chArea.getWidth() - 12) / 4;
    chorusOff_.setBounds (chArea.getX(), chArea.getY(), bw, 24);
    chorusI_.setBounds (chArea.getX() + bw + 4, chArea.getY(), bw, 24);
    chorusII_.setBounds (chArea.getX() + (bw + 4) * 2, chArea.getY(), bw, 24);
    chorusBoth_.setBounds (chArea.getX() + (bw + 4) * 3, chArea.getY(), bw, 24);

    keyboardStrip_.setBounds (16, h - 72, w - 32, 56);
}

} // namespace junovax::ui::celestial
