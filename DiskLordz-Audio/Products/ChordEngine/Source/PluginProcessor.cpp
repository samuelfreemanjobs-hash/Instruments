#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
int choiceParamIndex(const juce::AudioProcessorValueTreeState& apvts, const char* paramId)
{
    if (auto* choice = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(paramId)))
        return choice->getIndex();
    return 0;
}

constexpr const char* kRoot = "root";
constexpr const char* kScale = "scale";
constexpr const char* kPreset = "preset";
constexpr const char* kInversion = "inversion";
constexpr const char* kSpread = "spread";
constexpr const char* kSendHostMidi = "sendHostMidi";
constexpr const char* kAudition = "audition";
constexpr const char* kBeatDivider = "beatDivider";

constexpr double kDefaultBpm = 140.0;
constexpr double kPpqPerQuarter = 1.0;
} // namespace

ChordEngineAudioProcessor::ChordEngineAudioProcessor()
    : juce::AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , apvts_(*this, nullptr, "ChordEngine", createParameterLayout())
{
}

ChordEngineAudioProcessor::~ChordEngineAudioProcessor()
{
    juce::MidiBuffer dummy;
    stopHeldNotes(0, dummy);
    previewSynth_.allNotesOff();
}

juce::AudioProcessorValueTreeState::ParameterLayout ChordEngineAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        kRoot,
        "Root",
        juce::StringArray{ "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" },
        5));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        kScale,
        "Scale",
        juce::StringArray{ "Natural minor", "Harmonic minor", "Major", "Phrygian" },
        0));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        kPreset,
        "Progression",
        []() {
            juce::StringArray names;
            for (const auto& p : disklordz::chordengine::starterProgressions())
                names.add(p.displayName);
            return names;
        }(),
        0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(kInversion, "Inversion", 0, 3, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        kSpread, "Spread", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.35f));
    params.push_back(
        std::make_unique<juce::AudioParameterBool>(kSendHostMidi, "Send MIDI to host", true));
    params.push_back(std::make_unique<juce::AudioParameterBool>(kAudition, "Audition", false));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        kBeatDivider,
        "Bars per chord",
        juce::StringArray{ "1 bar", "2 bars" },
        0));

    return { params.begin(), params.end() };
}

void ChordEngineAudioProcessor::prepareToPlay(double sampleRate, int)
{
    sampleRate_ = sampleRate;
    previewSynth_.prepare(sampleRate);
}

void ChordEngineAudioProcessor::releaseResources()
{
    juce::MidiBuffer dummy;
    stopHeldNotes(0, dummy);
    previewSynth_.allNotesOff();
}

bool ChordEngineAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& out = layouts.getMainOutputChannelSet();
    return out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo();
}

int ChordEngineAudioProcessor::getRootPc() const
{
    return choiceParamIndex(apvts_, kRoot);
}

disklordz::theory::ScaleType ChordEngineAudioProcessor::getScale() const
{
    return static_cast<disklordz::theory::ScaleType>(
        juce::jlimit(0, 3, choiceParamIndex(apvts_, kScale)));
}

const disklordz::chordengine::StarterProgression& ChordEngineAudioProcessor::currentPreset() const
{
    const int idx = choiceParamIndex(apvts_, kPreset);
    const auto& all = disklordz::chordengine::starterProgressions();
    return all[static_cast<size_t>(juce::jlimit(0, disklordz::chordengine::kNumStarterProgressions - 1, idx))];
}

std::vector<int> ChordEngineAudioProcessor::notesForStep(int stepIndex) const
{
    const auto& preset = currentPreset();
    const auto& slot = preset.slots[static_cast<size_t>(juce::jlimit(0, kProgressionLength - 1, stepIndex))];
    const int inversion = static_cast<int>(apvts_.getRawParameterValue(kInversion)->load());
    const float spread = apvts_.getRawParameterValue(kSpread)->load();

    ScaleType scale = getScale();
    int rootPc = getRootPc();

    return disklordz::theory::buildVoicedChord(
        rootPc, scale, slot.degree1Based, slot.quality, inversion, spread, 52);
}

void ChordEngineAudioProcessor::stopHeldNotes(int sampleOffset, juce::MidiBuffer& midi)
{
    for (int note : heldNotes_)
    {
        previewSynth_.noteOff(note);
        if (apvts_.getRawParameterValue(kSendHostMidi)->load() > 0.5f)
            midi.addEvent(juce::MidiMessage::noteOff(1, note), sampleOffset);
    }
    heldNotes_.clear();
    activeStep_ = -1;
}

void ChordEngineAudioProcessor::startChord(int stepIndex, int sampleOffset, juce::MidiBuffer& midi)
{
    stopHeldNotes(sampleOffset, midi);

    const auto notes = notesForStep(stepIndex);
    if (notes.empty())
        return;

    activeStep_ = stepIndex;
    constexpr float velocity = 0.9f;

    for (int note : notes)
    {
        previewSynth_.noteOn(note, velocity);
        if (apvts_.getRawParameterValue(kSendHostMidi)->load() > 0.5f)
            midi.addEvent(juce::MidiMessage::noteOn(1, note, velocity), sampleOffset);
        heldNotes_.push_back(note);
    }
}

void ChordEngineAudioProcessor::advanceSequencer(double ppqPosition,
                                                  int /*numSamples*/,
                                                  double /*bpm*/,
                                                  juce::MidiBuffer& midi)
{
    const int barsPerChord = apvts_.getRawParameterValue(kBeatDivider)->load() < 0.5f ? 1 : 2;
    const double quartersPerChord = 4.0 * static_cast<double>(barsPerChord);

    const int step =
        static_cast<int>(std::floor(ppqPosition / quartersPerChord)) % kProgressionLength;

    if (step != lastTriggeredStep_)
    {
        startChord(step, 0, midi);
        lastTriggeredStep_ = step;
    }
}

void ChordEngineAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;

    // Consume external note-ons as “tap chord” preview (first 4 white keys conceptually).
    for (const auto metadata : midi)
    {
        const auto msg = metadata.getMessage();
        if (msg.isAllNotesOff() || msg.isAllSoundOff())
        {
            stopHeldNotes(0, midi);
            previewSynth_.allNotesOff();
        }
    }
    midi.clear();

    double bpm = kDefaultBpm;
    double ppqPosition = 0.0;
    bool hostPlaying = false;

    if (auto* playHead = getPlayHead())
    {
        if (auto opt = playHead->getPosition())
        {
            if (opt->getBpm().hasValue())
                bpm = *opt->getBpm();
            if (opt->getPpqPosition().hasValue())
                ppqPosition = *opt->getPpqPosition();
            hostPlaying = opt->getIsPlaying();
        }
    }

    auditionActive_ = apvts_.getRawParameterValue(kAudition)->load() > 0.5f;

    if (auditionActive_ && !hostPlaying)
    {
        const double samplesToPpq = (bpm / 60.0) / sampleRate_;
        auditionPpq_ += static_cast<double>(buffer.getNumSamples()) * samplesToPpq;
    }

    if (hostPlaying || auditionActive_)
    {
        const double timeline = hostPlaying ? ppqPosition : auditionPpq_;
        advanceSequencer(timeline, buffer.getNumSamples(), bpm, midi);
    }
    else if (lastTriggeredStep_ != -1)
    {
        stopHeldNotes(0, midi);
        lastTriggeredStep_ = -1;
    }

    previewSynth_.render(buffer);
}

juce::AudioProcessorEditor* ChordEngineAudioProcessor::createEditor()
{
    return new ChordEngineAudioProcessorEditor(*this);
}

int ChordEngineAudioProcessor::getNumPrograms()
{
    return disklordz::chordengine::kNumStarterProgressions;
}

int ChordEngineAudioProcessor::getCurrentProgram()
{
    return currentProgram_;
}

void ChordEngineAudioProcessor::setCurrentProgram(int index)
{
    currentProgram_ = juce::jlimit(0, getNumPrograms() - 1, index);
    const auto& preset = disklordz::chordengine::starterProgressions()
        [static_cast<size_t>(currentProgram_)];

    if (auto* root = apvts_.getParameter(kRoot))
        root->setValueNotifyingHost(static_cast<float>(preset.defaultRootPc) / 11.0f);
    if (auto* scale = apvts_.getParameter(kScale))
        scale->setValueNotifyingHost(
            static_cast<float>(static_cast<int>(preset.recommendedScale)) / 3.0f);

    if (auto* prog = apvts_.getParameter(kPreset))
        prog->setValueNotifyingHost(
            static_cast<float>(currentProgram_)
            / static_cast<float>(disklordz::chordengine::kNumStarterProgressions - 1));
}

const juce::String ChordEngineAudioProcessor::getProgramName(int index)
{
    const int i = juce::jlimit(0, getNumPrograms() - 1, index);
    return disklordz::chordengine::starterProgressions()[static_cast<size_t>(i)].displayName;
}

void ChordEngineAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts_.copyState();
    state.setProperty("currentProgram", currentProgram_, nullptr);
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ChordEngineAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml == nullptr || !xml->hasTagName(apvts_.state.getType()))
        return;

    apvts_.replaceState(juce::ValueTree::fromXml(*xml));
    currentProgram_ = static_cast<int>(apvts_.state.getProperty("currentProgram", 0));
}

void ChordEngineAudioProcessor::triggerAudition()
{
    auditionPpq_ = 0.0;
    lastTriggeredStep_ = -1;
    if (auto* p = apvts_.getParameter(kAudition))
        p->setValueNotifyingHost(1.0f);
}

std::array<juce::String, ChordEngineAudioProcessor::kProgressionLength>
ChordEngineAudioProcessor::getChordSymbolLabels() const
{
    std::array<juce::String, kProgressionLength> labels;
    const auto& preset = currentPreset();
    const int rootPc = getRootPc();
    const auto scale = getScale();

    for (int i = 0; i < kProgressionLength; ++i)
    {
        const auto& slot = preset.slots[static_cast<size_t>(i)];
        const int chordPc =
            disklordz::theory::chordRootPitchClass(rootPc, scale, slot.degree1Based);
        labels[static_cast<size_t>(i)] =
            juce::String(disklordz::theory::noteNameFromPc(chordPc))
            + disklordz::theory::chordQualitySymbol(slot.quality) + "  ("
            + disklordz::theory::degreeLabel(slot.degree1Based, slot.quality, scale) + ")";
    }
    return labels;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChordEngineAudioProcessor();
}