#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "FactoryPresets.h"

ProphetRev2TrapAudioProcessor::ProphetRev2TrapAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "ProphetRev2Trap", createParameterLayout())
{
    setCurrentProgram(0);
}

ProphetRev2TrapAudioProcessor::~ProphetRev2TrapAudioProcessor() = default;

const juce::String ProphetRev2TrapAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

juce::AudioProcessorValueTreeState::ParameterLayout ProphetRev2TrapAudioProcessor::createParameterLayout()
{
    auto choice = [](const char* id, const char* name, const juce::StringArray& c, int def) {
        return std::make_unique<juce::AudioParameterChoice>(id, name, c, def);
    };
    auto f = [](const char* id, const char* name, float lo, float hi, float def) {
        return std::make_unique<juce::AudioParameterFloat>(
            id, name, juce::NormalisableRange<float>(lo, hi, 0.001f, 1.0f), def);
    };

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    const juce::StringArray waves { "Saw", "Triangle", "Pulse" };
    p.push_back(choice("dco1_wave", "DCO 1 Wave", waves, 0));
    p.push_back(choice("dco2_wave", "DCO 2 Wave", waves, 0));
    p.push_back(f("dco_mix", "DCO Mix", 0.f, 1.f, 0.5f));
    p.push_back(f("pwm_width", "Pulse Width", 0.05f, 0.95f, 0.5f));
    p.push_back(f("sub_level", "Sub Level", 0.f, 1.f, 0.35f));
    p.push_back(f("slop", "Slop / Vintage", 0.f, 1.f, 0.3f));
    p.push_back(choice("filter_mode", "Filter Mode", { "24dB", "12dB" }, 0));
    p.push_back(f("filter_cutoff", "Filter Cutoff Hz", 80.f, 12000.f, 2500.f));
    p.push_back(f("filter_res", "Filter Resonance", 0.f, 1.f, 0.4f));
    p.push_back(f("bass_comp", "Bass Compensation", 0.f, 1.f, 0.75f));
    p.push_back(f("glide_ms", "Glide ms", 0.f, 500.f, 20.f));
    p.push_back(choice("saturation_on", "Saturation", { "Off", "On" }, 0));
    p.push_back(f("crunch", "Crunch", 0.f, 1.f, 0.3f));
    p.push_back(f("amp_a", "Amp Attack", 0.001f, 3.f, 0.01f));
    p.push_back(f("amp_d", "Amp Decay", 0.001f, 3.f, 0.2f));
    p.push_back(f("amp_s", "Amp Sustain", 0.f, 1.f, 0.7f));
    p.push_back(f("amp_r", "Amp Release", 0.001f, 5.f, 0.25f));
    p.push_back(f("filt_a", "Filter Attack", 0.001f, 3.f, 0.01f));
    p.push_back(f("filt_d", "Filter Decay", 0.001f, 3.f, 0.3f));
    p.push_back(f("filt_s", "Filter Sustain", 0.f, 1.f, 0.5f));
    p.push_back(f("filt_r", "Filter Release", 0.001f, 5.f, 0.3f));
    p.push_back(f("master_gain", "Master Gain", 0.f, 1.f, 0.85f));
    return { p.begin(), p.end() };
}

void ProphetRev2TrapAudioProcessor::prepareToPlay(double sampleRate, int)
{
    engine_.prepareToPlay(sampleRate);
}

void ProphetRev2TrapAudioProcessor::releaseResources() {}

bool ProphetRev2TrapAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& out = layouts.getMainOutputChannelSet();
    return out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo();
}

void ProphetRev2TrapAudioProcessor::pushParametersToEngine()
{
    const auto lf = [this](const char* id) { return apvts_.getRawParameterValue(id)->load(); };

    engine_.setWave1(static_cast<int>(lf("dco1_wave")));
    engine_.setWave2(static_cast<int>(lf("dco2_wave")));
    engine_.setDcoMix(lf("dco_mix"));
    engine_.setPwm(lf("pwm_width"));
    engine_.setSubLevel(lf("sub_level"));
    engine_.setSlop(lf("slop"));
    engine_.setFilterMode(static_cast<int>(lf("filter_mode")) == 0 ? 0 : 1);
    engine_.setCutoff(lf("filter_cutoff"));
    engine_.setResonance(lf("filter_res"));
    engine_.setBassComp(lf("bass_comp"));
    engine_.setGlide(lf("glide_ms") * 0.001f);
    engine_.setSaturation(static_cast<int>(lf("saturation_on")));
    engine_.setCrunch(lf("crunch"));
    engine_.setAmpAdsr(lf("amp_a"), lf("amp_d"), lf("amp_s"), lf("amp_r"));
    engine_.setFilterAdsr(lf("filt_a"), lf("filt_d"), lf("filt_s"), lf("filt_r"));
}

void ProphetRev2TrapAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    pushParametersToEngine();

    for (const auto meta : midi)
    {
        const auto msg = meta.getMessage();
        if (msg.isNoteOn())
            engine_.noteOn(msg.getNoteNumber(), msg.getFloatVelocity());
        else if (msg.isNoteOff())
            engine_.noteOff(msg.getNoteNumber());
    }

    const float master = apvts_.getRawParameterValue("master_gain")->load();
    auto* left = buffer.getWritePointer(0);
    auto* right = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;
    engine_.processBlock(left, right, buffer.getNumSamples());

    buffer.applyGain(master);
}

int ProphetRev2TrapAudioProcessor::getNumPrograms()
{
    return rev2presets::kNumPrograms;
}

int ProphetRev2TrapAudioProcessor::getCurrentProgram()
{
    return currentProgram_;
}

void ProphetRev2TrapAudioProcessor::setCurrentProgram(int index)
{
    currentProgram_ = juce::jlimit(0, getNumPrograms() - 1, index);
    engine_.loadPreset(currentProgram_);
    rev2presets::applyPatch(apvts_, currentProgram_);
}

const juce::String ProphetRev2TrapAudioProcessor::getProgramName(int index)
{
    if (index < 0 || index >= rev2presets::kNumPrograms)
        return {};
    return rev2presets::kPatches[static_cast<std::size_t>(index)].name;
}

juce::AudioProcessorEditor* ProphetRev2TrapAudioProcessor::createEditor()
{
    return new ProphetRev2TrapAudioProcessorEditor(*this);
}

void ProphetRev2TrapAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos(destData, false);
    mos.writeInt(currentProgram_);
    apvts_.state.writeToStream(mos);
}

void ProphetRev2TrapAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    juce::MemoryInputStream mis(data, static_cast<size_t>(sizeInBytes), false);
    currentProgram_ = mis.readInt();
    apvts_.state = juce::ValueTree::readFromStream(mis);
    engine_.loadPreset(currentProgram_);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ProphetRev2TrapAudioProcessor();
}
