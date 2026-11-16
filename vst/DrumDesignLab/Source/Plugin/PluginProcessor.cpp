#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cstring>

namespace
{
template <typename T>
T* raw(juce::AudioProcessorValueTreeState& apvts, const char* id)
{
    return dynamic_cast<T*>(apvts.getParameter(id));
}
} // namespace

DrumDesignLabAudioProcessor::DrumDesignLabAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "DDL", createParameterLayout())
{
    engine_ = std::make_unique<disklordz::MultiGenreDrumEngine>();
}

juce::AudioProcessorValueTreeState::ParameterLayout DrumDesignLabAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    juce::StringArray styleNames;
    for (int i = 0; i < static_cast<int>(disklordz::KitStyle::Count); ++i)
        styleNames.add(disklordz::kitStyleName(static_cast<disklordz::KitStyle>(i)));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "ddl_kit_style", 1 }, "Kit Style", styleNames, 1));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_output_gain", 1 }, "Output",
        juce::NormalisableRange<float>(0.0f, 1.5f, 0.01f), 0.95f));

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID { "ddl_selected_pad", 1 }, "Selected Pad", 1, 16, 1));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_sub_freq_hz", 1 }, "Sub Frequency",
        juce::NormalisableRange<float>(35.f, 75.f, 0.1f), 49.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_punch_freq_hz", 1 }, "Initial Punch",
        juce::NormalisableRange<float>(80.f, 250.f, 0.1f), 165.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_pitch_sweep_ms", 1 }, "Pitch Sweep",
        juce::NormalisableRange<float>(5.f, 100.f, 0.1f), 24.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_amp_decay_ms", 1 }, "Amp Decay",
        juce::NormalisableRange<float>(20.f, 2500.f, 1.f, 0.4f), 340.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_body_level", 1 }, "Body Layer",
        juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 85.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_attack_level", 1 }, "Attack Layer",
        juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 70.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_texture_level", 1 }, "Texture Layer",
        juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 50.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_drive", 1 }, "Drive",
        juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 35.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_dirt_blend", 1 }, "Dirt Blend",
        juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 55.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_sampler_grit", 1 }, "12-Bit Sampler Grit",
        juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_stereo_width", 1 }, "Stereo Width",
        juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 30.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_reverb_mix", 1 }, "Space Mix",
        juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 10.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_filter_cutoff_hz", 1 }, "Filter Cutoff",
        juce::NormalisableRange<float>(40.f, 18000.f, 1.f, 0.3f), 9500.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_fm_ratio", 1 }, "FM Ratio",
        juce::NormalisableRange<float>(0.5f, 8.f, 0.01f), 2.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_fm_index", 1 }, "FM Index",
        juce::NormalisableRange<float>(0.f, 6.f, 0.01f), 2.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "ddl_pad_gain", 1 }, "Pad Gain",
        juce::NormalisableRange<float>(0.1f, 2.f, 0.01f), 1.f));

    return { params.begin(), params.end() };
}

disklordz::sound_edit::Values DrumDesignLabAudioProcessor::readEditValuesFromApvts() const
{
    auto load = [this](const char* id) {
        return apvts.getRawParameterValue(id)->load();
    };
    disklordz::sound_edit::Values v;
    v.subHz = load("ddl_sub_freq_hz");
    v.punchHz = load("ddl_punch_freq_hz");
    v.sweepMs = load("ddl_pitch_sweep_ms");
    v.decayMs = load("ddl_amp_decay_ms");
    v.bodyLevel = load("ddl_body_level") * 0.01f;
    v.attackLevel = load("ddl_attack_level") * 0.01f;
    v.textureLevel = load("ddl_texture_level") * 0.01f;
    v.drive01 = load("ddl_drive") * 0.01f;
    v.dirt01 = load("ddl_dirt_blend") * 0.01f;
    v.grit01 = load("ddl_sampler_grit") * 0.01f;
    v.width01 = load("ddl_stereo_width") * 0.01f;
    v.reverb01 = load("ddl_reverb_mix") * 0.01f;
    v.cutoffHz = load("ddl_filter_cutoff_hz");
    v.fmRatio = load("ddl_fm_ratio");
    v.fmIndex = load("ddl_fm_index");
    v.padGain = load("ddl_pad_gain");
    return v;
}

void DrumDesignLabAudioProcessor::writeEditValuesToApvts(const disklordz::sound_edit::Values& v)
{
    suppressEditPush_ = true;
    auto setFloat = [this](const char* id, float value) {
        if (auto* p = apvts.getParameter(id))
            p->setValueNotifyingHost(p->convertTo0to1(value));
    };
    setFloat("ddl_sub_freq_hz", v.subHz);
    setFloat("ddl_punch_freq_hz", v.punchHz);
    setFloat("ddl_pitch_sweep_ms", v.sweepMs);
    setFloat("ddl_amp_decay_ms", v.decayMs);
    setFloat("ddl_body_level", v.bodyLevel * 100.f);
    setFloat("ddl_attack_level", v.attackLevel * 100.f);
    setFloat("ddl_texture_level", v.textureLevel * 100.f);
    setFloat("ddl_drive", v.drive01 * 100.f);
    setFloat("ddl_dirt_blend", v.dirt01 * 100.f);
    setFloat("ddl_sampler_grit", v.grit01 * 100.f);
    setFloat("ddl_stereo_width", v.width01 * 100.f);
    setFloat("ddl_reverb_mix", v.reverb01 * 100.f);
    setFloat("ddl_filter_cutoff_hz", v.cutoffHz);
    setFloat("ddl_fm_ratio", v.fmRatio);
    setFloat("ddl_fm_index", v.fmIndex);
    setFloat("ddl_pad_gain", v.padGain);
    suppressEditPush_ = false;
}

void DrumDesignLabAudioProcessor::applySoundEditsFromApvts()
{
    if (suppressEditPush_ || engine_ == nullptr)
        return;
    const auto v = readEditValuesFromApvts();
    if (hasLastApplied_
        && std::memcmp(&v, &lastApplied_, sizeof(v)) == 0)
        return;
    lastApplied_ = v;
    hasLastApplied_ = true;
    engine_->applySoundEdit(selectedPad_, v);
}

void DrumDesignLabAudioProcessor::pullKnobsFromEngine()
{
    if (engine_ == nullptr)
        return;
    writeEditValuesToApvts(engine_->readSoundEdit(selectedPad_));
    hasLastApplied_ = false;
}

void DrumDesignLabAudioProcessor::selectPad(int padIndex, bool preview)
{
    selectedPad_ = juce::jlimit(0, 15, padIndex);
    if (auto* p = raw<juce::AudioParameterInt>(apvts, "ddl_selected_pad"))
        p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(selectedPad_ + 1)));

    hasLastApplied_ = false;
    if (engine_ != nullptr)
        writeEditValuesToApvts(engine_->readSoundEdit(selectedPad_));

    if (preview)
        queueUiNote(36 + selectedPad_, 0.85f);
}

void DrumDesignLabAudioProcessor::queueUiNote(int midiNote, float velocity01) noexcept
{
    uiVel_.store(velocity01, std::memory_order_relaxed);
    uiNote_.store(midiNote, std::memory_order_release);
}

void DrumDesignLabAudioProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    if (engine_ == nullptr)
        engine_ = std::make_unique<disklordz::MultiGenreDrumEngine>();
    engine_->prepare(sampleRate);
    engine_->setStyle(currentStyle_);
    writeEditValuesToApvts(engine_->readSoundEdit(selectedPad_));
}

bool DrumDesignLabAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& main = layouts.getMainOutputChannelSet();
    return main == juce::AudioChannelSet::mono() || main == juce::AudioChannelSet::stereo();
}

void DrumDesignLabAudioProcessor::setKitStyle(disklordz::KitStyle style)
{
    currentStyle_ = style;
    if (engine_ != nullptr)
        engine_->setStyle(style);
    if (auto* p = apvts.getParameter("ddl_kit_style"))
        p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(static_cast<int>(style))));
    if (engine_ != nullptr)
        writeEditValuesToApvts(engine_->readSoundEdit(selectedPad_));
}

void DrumDesignLabAudioProcessor::setCurrentProgram(int index)
{
    const int n = static_cast<int>(disklordz::KitStyle::Count);
    setKitStyle(static_cast<disklordz::KitStyle>(juce::jlimit(0, n - 1, index)));
}

const juce::String DrumDesignLabAudioProcessor::getProgramName(int index)
{
    const int n = static_cast<int>(disklordz::KitStyle::Count);
    if (index < 0 || index >= n)
        return {};
    return disklordz::kitStyleName(static_cast<disklordz::KitStyle>(index));
}

void DrumDesignLabAudioProcessor::syncStyleFromApvts()
{
    if (auto* choice = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("ddl_kit_style")))
    {
        const auto style = static_cast<disklordz::KitStyle>(choice->getIndex());
        if (style != currentStyle_)
        {
            currentStyle_ = style;
            if (engine_ != nullptr)
                engine_->setStyle(style);
            hasLastApplied_ = false;
            knobsNeedPull_.store(true, std::memory_order_release);
        }
    }
    if (auto* padParam = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter("ddl_selected_pad")))
    {
        const int pad = juce::jlimit(0, 15, padParam->get() - 1);
        if (pad != selectedPad_)
        {
            selectedPad_ = pad;
            hasLastApplied_ = false;
            knobsNeedPull_.store(true, std::memory_order_release);
        }
    }
}

void DrumDesignLabAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    if (engine_ == nullptr)
        return;

    syncStyleFromApvts();
    if (! suppressEditPush_)
        applySoundEditsFromApvts();

    const float outGain = apvts.getRawParameterValue("ddl_output_gain")->load();
    const int n = buffer.getNumSamples();
    auto* L = buffer.getWritePointer(0);
    auto* R = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

    const int pending = uiNote_.exchange(-1, std::memory_order_acq_rel);
    if (pending >= 0)
        engine_->handleMidiNote(pending, uiVel_.load(std::memory_order_relaxed));

    auto it = midi.findNextSamplePosition(0);
    for (int i = 0; i < n; ++i)
    {
        while (it != midi.cend() && (*it).samplePosition <= i)
        {
            const auto msg = (*it).getMessage();
            if (msg.isNoteOn())
                engine_->handleMidiNote(msg.getNoteNumber(), msg.getFloatVelocity());
            ++it;
        }
        float l = 0.f, r = 0.f;
        engine_->renderTick(l, r);
        L[i] = l * outGain;
        if (R != nullptr)
            R[i] = r * outGain;
    }
}

void DrumDesignLabAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary(*xml, destData);
}

void DrumDesignLabAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        if (xml->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor* DrumDesignLabAudioProcessor::createEditor()
{
    return new DrumDesignLabAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DrumDesignLabAudioProcessor();
}
