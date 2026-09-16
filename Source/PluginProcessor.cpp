#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "Assets/FactoryPatchLibrary.h"
#include "BinaryData.h"
#include "Preset/ApvtsBridge.h"
#include "Parameters/EnvelopeParameters.h"
#include "Parameters/EffectParameters.h"
#include "Parameters/FilterParameters.h"
#include "Preset/JdPatchLayout.h"

#include <cstdlib>

namespace
{
constexpr const char* kMasterGainId = "masterGain";
constexpr const char* kTone1LevelId = "tone1Level";
constexpr const char* kTone2LevelId = "tone2Level";
constexpr const char* kTone3LevelId = "tone3Level";
constexpr const char* kTone4LevelId = "tone4Level";
constexpr const char* kFilterResonanceId = "filterResonance";
constexpr const char* kCouplingModeId = "couplingMode";
constexpr const char* kGroupADriveId = "groupADrive";
constexpr const char* kGroupBMixId = "groupBMix";
constexpr const char* kTone1WaveId = "tone1Wave";
constexpr const char* kTone2WaveId = "tone2Wave";
constexpr const char* kTone3WaveId = "tone3Wave";
constexpr const char* kTone4WaveId = "tone4Wave";
constexpr const char* kTone1MultisampleId = "tone1Multisample";
constexpr const char* kTone2MultisampleId = "tone2Multisample";
constexpr const char* kTone3MultisampleId = "tone3Multisample";
constexpr const char* kTone4MultisampleId = "tone4Multisample";
constexpr const char* kTone1MuteId = "tone1Mute";
constexpr const char* kTone2MuteId = "tone2Mute";
constexpr const char* kTone3MuteId = "tone3Mute";
constexpr const char* kTone4MuteId = "tone4Mute";
constexpr const char* kAmpAttackId = "ampAttack";
constexpr const char* kAmpDecayId = "ampDecay";
constexpr const char* kAmpSustainId = "ampSustain";
constexpr const char* kAmpReleaseId = "ampRelease";
constexpr const char* kFilterAttackId = "filterAttack";
constexpr const char* kFilterDecayId = "filterDecay";
constexpr const char* kFilterSustainId = "filterSustain";
constexpr const char* kFilterReleaseId = "filterRelease";

juce::NormalisableRange<float> envelopeTimeRange() noexcept
{
    return { jdupgraded::dsp::kMinEnvelopeTimeSec, 15.0f, 0.001f, 0.35f };
}

jdupgraded::dsp::ToneCouplingMode couplingFromIndex (int index) noexcept
{
    switch (index)
    {
        case 1: return jdupgraded::dsp::ToneCouplingMode::ringPair01;
        case 2: return jdupgraded::dsp::ToneCouplingMode::ringPair23;
        case 3: return jdupgraded::dsp::ToneCouplingMode::crossModPair01;
        case 4: return jdupgraded::dsp::ToneCouplingMode::crossModPair23;
        case 5: return jdupgraded::dsp::ToneCouplingMode::hardSyncPair01;
        case 6: return jdupgraded::dsp::ToneCouplingMode::hardSyncPair23;
        default: return jdupgraded::dsp::ToneCouplingMode::independent;
    }
}
} // namespace

JDUpgradedAudioProcessor::JDUpgradedAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_ (*this, nullptr, "Parameters", createParameterLayout())
{
    romLoader_.loadFactoryRom (BinaryData::jdupg_cleanroom_rom,
                               BinaryData::jdupg_cleanroom_romSize);

    if (const char* externalRom = std::getenv ("JDUPGRADED_ROM_PATH"))
    {
        if (! romLoader_.loadUserRomFile (externalRom))
            juce::Logger::writeToLog ("JDUPGRADED_ROM_PATH failed: "
                                      + juce::String (romLoader_.getLastUserLoadError()));
    }

    refreshCachedParameters();
    applyFactoryPatch (0);
}

JDUpgradedAudioProcessor::~JDUpgradedAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout JDUpgradedAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { kMasterGainId, 1 }, "Master Gain",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.75f));

    for (int i = 1; i <= 4; ++i)
    {
        const juce::String id = "tone" + juce::String (i) + "Level";
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { id, 1 }, "Tone " + juce::String (i) + " Level",
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), i == 1 ? 1.0f : 0.0f));
    }

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { kFilterResonanceId, 1 }, "Filter Resonance",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.35f));

    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { jdupgraded::params::kFilterLinkId, 1 }, "Link Filter Res", true));

    const juce::NormalisableRange<float> tvfRange (0.0f, 1.0f, 0.001f);
    for (int tone = 1; tone <= 4; ++tone)
    {
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { jdupgraded::params::toneFilterParamId (tone, "Cutoff"), 1 },
            "Tone " + juce::String (tone) + " Cutoff", tvfRange, 1.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { jdupgraded::params::toneFilterParamId (tone, "Resonance"), 1 },
            "Tone " + juce::String (tone) + " Resonance", tvfRange, 0.35f));
    }

    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { kCouplingModeId, 1 }, "Tone Coupling",
        juce::StringArray { "Independent", "Ring 1-2", "Ring 3-4", "X-Mod 1-2", "X-Mod 3-4", "Sync 1-2", "Sync 3-4" },
        0));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { kGroupADriveId, 1 }, "Group A Drive",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { kGroupBMixId, 1 }, "Group B Space",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.0f));

    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { jdupgraded::params::kGroupAEnableId, 1 }, "Group A On", true));
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { jdupgraded::params::kGroupBEnableId, 1 }, "Group B On", true));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { jdupgraded::params::kGroupBChorusId, 1 }, "Group B Chorus",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.0f));

    const int maxWave = static_cast<int> (jdupgraded::assets::kCleanroomWaveCount) - 1;
    params.push_back (std::make_unique<juce::AudioParameterInt> (
        juce::ParameterID { kTone1WaveId, 1 }, "Tone 1 Wave", 0, maxWave, 0));
    params.push_back (std::make_unique<juce::AudioParameterInt> (
        juce::ParameterID { kTone2WaveId, 1 }, "Tone 2 Wave", 0, maxWave, 16));
    params.push_back (std::make_unique<juce::AudioParameterInt> (
        juce::ParameterID { kTone3WaveId, 1 }, "Tone 3 Wave", 0, maxWave, 32));
    params.push_back (std::make_unique<juce::AudioParameterInt> (
        juce::ParameterID { kTone4WaveId, 1 }, "Tone 4 Wave", 0, maxWave, 48));

    const int maxMs = static_cast<int> (jdupgraded::assets::kMultisampleSetCount);
    params.push_back (std::make_unique<juce::AudioParameterInt> (
        juce::ParameterID { kTone1MultisampleId, 1 }, "Tone 1 Multisample", 0, maxMs, 0));
    params.push_back (std::make_unique<juce::AudioParameterInt> (
        juce::ParameterID { kTone2MultisampleId, 1 }, "Tone 2 Multisample", 0, maxMs, 0));
    params.push_back (std::make_unique<juce::AudioParameterInt> (
        juce::ParameterID { kTone3MultisampleId, 1 }, "Tone 3 Multisample", 0, maxMs, 0));
    params.push_back (std::make_unique<juce::AudioParameterInt> (
        juce::ParameterID { kTone4MultisampleId, 1 }, "Tone 4 Multisample", 0, maxMs, 0));

    for (int i = 1; i <= 4; ++i)
    {
        const juce::String id = "tone" + juce::String (i) + "Mute";
        params.push_back (std::make_unique<juce::AudioParameterBool> (
            juce::ParameterID { id, 1 }, "Tone " + juce::String (i) + " Mute", false));
    }

    const auto timeRange = envelopeTimeRange();
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { kAmpAttackId, 1 }, "Amp Attack", timeRange, 0.005f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { kAmpDecayId, 1 }, "Amp Decay", timeRange, 0.2f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { kAmpSustainId, 1 }, "Amp Sustain",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.85f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { kAmpReleaseId, 1 }, "Amp Release", timeRange, 0.35f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { kFilterAttackId, 1 }, "Filter Attack", timeRange, 0.008f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { kFilterDecayId, 1 }, "Filter Decay", timeRange, 0.25f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { kFilterSustainId, 1 }, "Filter Sustain",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.65f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { kFilterReleaseId, 1 }, "Filter Release", timeRange, 0.4f));

    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { jdupgraded::params::kEnvelopeLinkId, 1 }, "Link Envelopes", true));

    const juce::NormalisableRange<float> sustainRange (0.0f, 1.0f, 0.001f);
    for (int tone = 1; tone <= 4; ++tone)
    {
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { jdupgraded::params::toneEnvelopeParamId (tone, "AmpAttack"), 1 },
            "Tone " + juce::String (tone) + " Amp A", timeRange, 0.005f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { jdupgraded::params::toneEnvelopeParamId (tone, "AmpDecay"), 1 },
            "Tone " + juce::String (tone) + " Amp D", timeRange, 0.2f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { jdupgraded::params::toneEnvelopeParamId (tone, "AmpSustain"), 1 },
            "Tone " + juce::String (tone) + " Amp S", sustainRange, 0.85f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { jdupgraded::params::toneEnvelopeParamId (tone, "AmpRelease"), 1 },
            "Tone " + juce::String (tone) + " Amp R", timeRange, 0.35f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { jdupgraded::params::toneEnvelopeParamId (tone, "FilterAttack"), 1 },
            "Tone " + juce::String (tone) + " Flt A", timeRange, 0.008f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { jdupgraded::params::toneEnvelopeParamId (tone, "FilterDecay"), 1 },
            "Tone " + juce::String (tone) + " Flt D", timeRange, 0.25f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { jdupgraded::params::toneEnvelopeParamId (tone, "FilterSustain"), 1 },
            "Tone " + juce::String (tone) + " Flt S", sustainRange, 0.65f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { jdupgraded::params::toneEnvelopeParamId (tone, "FilterRelease"), 1 },
            "Tone " + juce::String (tone) + " Flt R", timeRange, 0.4f));
    }

    return { params.begin(), params.end() };
}

void JDUpgradedAudioProcessor::refreshCachedParameters() noexcept
{
    masterGainPtr_ = apvts_.getRawParameterValue (kMasterGainId);
    tone1LevelPtr_ = apvts_.getRawParameterValue (kTone1LevelId);
    tone2LevelPtr_ = apvts_.getRawParameterValue (kTone2LevelId);
    tone3LevelPtr_ = apvts_.getRawParameterValue (kTone3LevelId);
    tone4LevelPtr_ = apvts_.getRawParameterValue (kTone4LevelId);
    filterResonancePtr_ = apvts_.getRawParameterValue (kFilterResonanceId);
    filterLinkPtr_ = apvts_.getRawParameterValue (jdupgraded::params::kFilterLinkId);
    couplingModePtr_ = apvts_.getRawParameterValue (kCouplingModeId);
    groupADrivePtr_ = apvts_.getRawParameterValue (kGroupADriveId);
    groupBMixPtr_ = apvts_.getRawParameterValue (kGroupBMixId);
    groupAEnablePtr_ = apvts_.getRawParameterValue (jdupgraded::params::kGroupAEnableId);
    groupBEnablePtr_ = apvts_.getRawParameterValue (jdupgraded::params::kGroupBEnableId);
    groupBChorusPtr_ = apvts_.getRawParameterValue (jdupgraded::params::kGroupBChorusId);
    tone1WavePtr_ = apvts_.getRawParameterValue (kTone1WaveId);
    tone2WavePtr_ = apvts_.getRawParameterValue (kTone2WaveId);
    tone3WavePtr_ = apvts_.getRawParameterValue (kTone3WaveId);
    tone4WavePtr_ = apvts_.getRawParameterValue (kTone4WaveId);
    tone1MultisamplePtr_ = apvts_.getRawParameterValue (kTone1MultisampleId);
    tone2MultisamplePtr_ = apvts_.getRawParameterValue (kTone2MultisampleId);
    tone3MultisamplePtr_ = apvts_.getRawParameterValue (kTone3MultisampleId);
    tone4MultisamplePtr_ = apvts_.getRawParameterValue (kTone4MultisampleId);
    tone1MutePtr_ = apvts_.getRawParameterValue (kTone1MuteId);
    tone2MutePtr_ = apvts_.getRawParameterValue (kTone2MuteId);
    tone3MutePtr_ = apvts_.getRawParameterValue (kTone3MuteId);
    tone4MutePtr_ = apvts_.getRawParameterValue (kTone4MuteId);
    ampAttackPtr_ = apvts_.getRawParameterValue (kAmpAttackId);
    ampDecayPtr_ = apvts_.getRawParameterValue (kAmpDecayId);
    ampSustainPtr_ = apvts_.getRawParameterValue (kAmpSustainId);
    ampReleasePtr_ = apvts_.getRawParameterValue (kAmpReleaseId);
    filterAttackPtr_ = apvts_.getRawParameterValue (kFilterAttackId);
    filterDecayPtr_ = apvts_.getRawParameterValue (kFilterDecayId);
    filterSustainPtr_ = apvts_.getRawParameterValue (kFilterSustainId);
    filterReleasePtr_ = apvts_.getRawParameterValue (kFilterReleaseId);
    envelopeLinkPtr_ = apvts_.getRawParameterValue (jdupgraded::params::kEnvelopeLinkId);

    for (int tone = 0; tone < 4; ++tone)
    {
        const int oneBased = tone + 1;
        toneFilterCutoffPtrs_[static_cast<std::size_t> (tone)] = apvts_.getRawParameterValue (
            jdupgraded::params::toneFilterParamId (oneBased, "Cutoff"));
        toneFilterResonancePtrs_[static_cast<std::size_t> (tone)] = apvts_.getRawParameterValue (
            jdupgraded::params::toneFilterParamId (oneBased, "Resonance"));

        auto& ptrs = toneEnvelopePtrs_[static_cast<std::size_t> (tone)];
        ptrs.ampAttack = apvts_.getRawParameterValue (
            jdupgraded::params::toneEnvelopeParamId (oneBased, "AmpAttack"));
        ptrs.ampDecay = apvts_.getRawParameterValue (
            jdupgraded::params::toneEnvelopeParamId (oneBased, "AmpDecay"));
        ptrs.ampSustain = apvts_.getRawParameterValue (
            jdupgraded::params::toneEnvelopeParamId (oneBased, "AmpSustain"));
        ptrs.ampRelease = apvts_.getRawParameterValue (
            jdupgraded::params::toneEnvelopeParamId (oneBased, "AmpRelease"));
        ptrs.filterAttack = apvts_.getRawParameterValue (
            jdupgraded::params::toneEnvelopeParamId (oneBased, "FilterAttack"));
        ptrs.filterDecay = apvts_.getRawParameterValue (
            jdupgraded::params::toneEnvelopeParamId (oneBased, "FilterDecay"));
        ptrs.filterSustain = apvts_.getRawParameterValue (
            jdupgraded::params::toneEnvelopeParamId (oneBased, "FilterSustain"));
        ptrs.filterRelease = apvts_.getRawParameterValue (
            jdupgraded::params::toneEnvelopeParamId (oneBased, "FilterRelease"));
    }
}

bool JDUpgradedAudioProcessor::isEnvelopeLinked() const noexcept
{
    return envelopeLinkPtr_ == nullptr || envelopeLinkPtr_->load() >= 0.5f;
}

bool JDUpgradedAudioProcessor::isFilterLinked() const noexcept
{
    return filterLinkPtr_ == nullptr || filterLinkPtr_->load() >= 0.5f;
}

void JDUpgradedAudioProcessor::copyGlobalFilterResonanceToAllTones() noexcept
{
    const float res = filterResonancePtr_ != nullptr ? filterResonancePtr_->load() : 0.35f;
    for (int tone = 1; tone <= 4; ++tone)
    {
        const auto id = jdupgraded::params::toneFilterParamId (tone, "Resonance");
        setApvtsFloat (id.toRawUTF8(), res);
    }
}

void JDUpgradedAudioProcessor::copyGlobalEnvelopesToAllTones() noexcept
{
    const float values[8] = {
        ampAttackPtr_ != nullptr ? ampAttackPtr_->load() : 0.005f,
        ampDecayPtr_ != nullptr ? ampDecayPtr_->load() : 0.2f,
        ampSustainPtr_ != nullptr ? ampSustainPtr_->load() : 0.85f,
        ampReleasePtr_ != nullptr ? ampReleasePtr_->load() : 0.35f,
        filterAttackPtr_ != nullptr ? filterAttackPtr_->load() : 0.008f,
        filterDecayPtr_ != nullptr ? filterDecayPtr_->load() : 0.25f,
        filterSustainPtr_ != nullptr ? filterSustainPtr_->load() : 0.65f,
        filterReleasePtr_ != nullptr ? filterReleasePtr_->load() : 0.4f,
    };

    const char* suffixes[8] = { "AmpAttack", "AmpDecay", "AmpSustain", "AmpRelease",
                                "FilterAttack", "FilterDecay", "FilterSustain", "FilterRelease" };

    for (int tone = 1; tone <= 4; ++tone)
    {
        for (int i = 0; i < 8; ++i)
        {
            const auto id = jdupgraded::params::toneEnvelopeParamId (tone, suffixes[i]);
            setApvtsFloat (id.toRawUTF8(), values[i]);
        }
    }
}

void JDUpgradedAudioProcessor::applyEnvelopeDefaultsForProgram (int programIndex) noexcept
{
    const int bucket = programIndex / 16;
    float ampA = 0.005f, ampD = 0.2f, ampS = 0.85f, ampR = 0.35f;
    float fltA = 0.008f, fltD = 0.25f, fltS = 0.65f, fltR = 0.4f;

    switch (bucket)
    {
        case 0: // EP
            ampA = 0.002f; ampD = 0.45f; ampS = 0.25f; ampR = 0.5f;
            fltD = 0.35f; fltS = 0.4f;
            break;
        case 1: // Pad
            ampA = 0.35f; ampD = 0.6f; ampS = 0.9f; ampR = 1.2f;
            fltA = 0.25f; fltS = 0.75f; fltR = 0.9f;
            break;
        case 2: // Bass
            ampA = 0.003f; ampD = 0.12f; ampS = 0.7f; ampR = 0.18f;
            fltA = 0.004f; fltD = 0.15f; fltS = 0.55f; fltR = 0.2f;
            break;
        case 3: // Vapor
            ampA = 0.08f; ampD = 0.5f; ampS = 0.8f; ampR = 0.8f;
            break;
        default:
            break;
    }

    setApvtsFloat (kAmpAttackId, ampA);
    setApvtsFloat (kAmpDecayId, ampD);
    setApvtsFloat (kAmpSustainId, ampS);
    setApvtsFloat (kAmpReleaseId, ampR);
    setApvtsFloat (kFilterAttackId, fltA);
    setApvtsFloat (kFilterDecayId, fltD);
    setApvtsFloat (kFilterSustainId, fltS);
    setApvtsFloat (kFilterReleaseId, fltR);
    copyGlobalEnvelopesToAllTones();

    for (int tone = 1; tone <= 4; ++tone)
    {
        const float toneScale = 1.0f + 0.04f * static_cast<float> (tone - 1);
        setApvtsFloat (jdupgraded::params::toneEnvelopeParamId (tone, "AmpDecay").toRawUTF8(), ampD * toneScale);
        setApvtsFloat (jdupgraded::params::toneEnvelopeParamId (tone, "FilterDecay").toRawUTF8(), fltD * toneScale);
    }
}

int JDUpgradedAudioProcessor::getNumPrograms()
{
    return static_cast<int> (jdupgraded::assets::FactoryPatchLibrary::getPatchCount());
}

int JDUpgradedAudioProcessor::getCurrentProgram()
{
    return currentProgram_;
}

void JDUpgradedAudioProcessor::setCurrentProgram (int index)
{
    const auto count = jdupgraded::assets::FactoryPatchLibrary::getPatchCount();
    currentProgram_ = juce::jlimit (0, static_cast<int> (count) - 1, index);
    applyFactoryPatch (currentProgram_);
}

const juce::String JDUpgradedAudioProcessor::getProgramName (int index)
{
    return juce::String (jdupgraded::assets::FactoryPatchLibrary::getPatch (static_cast<std::size_t> (index)).name);
}

void JDUpgradedAudioProcessor::setApvtsFloat (const char* paramId, float value)
{
    if (auto* param = apvts_.getParameter (paramId))
        param->setValueNotifyingHost (param->convertTo0to1 (value));
}

void JDUpgradedAudioProcessor::setApvtsInt (const char* paramId, int value)
{
    if (auto* param = apvts_.getParameter (paramId))
        param->setValueNotifyingHost (param->convertTo0to1 (static_cast<float> (value)));
}

void JDUpgradedAudioProcessor::setApvtsChoice (const char* paramId, int index)
{
    if (auto* param = dynamic_cast<juce::AudioParameterChoice*> (apvts_.getParameter (paramId)))
        param->setValueNotifyingHost (param->convertTo0to1 (static_cast<float> (index)));
}

void JDUpgradedAudioProcessor::setApvtsBool (const char* paramId, bool value)
{
    if (auto* param = apvts_.getParameter (paramId))
        param->setValueNotifyingHost (value ? 1.0f : 0.0f);
}

void JDUpgradedAudioProcessor::applyFactoryPatch (int index)
{
    const auto& patch = jdupgraded::assets::FactoryPatchLibrary::getPatch (static_cast<std::size_t> (index));

    for (auto& mod : tonePitchMod_)
        mod = jdupgraded::preset::JdTonePitchMod{};
    toneFineCents_.fill (0.0f);

    setApvtsFloat (kMasterGainId, patch.masterGain);
    setApvtsFloat (kGroupADriveId, patch.groupADrive);
    setApvtsFloat (kGroupBMixId, patch.groupBMix);

    int couplingIndex = 0;
    switch (patch.coupling)
    {
        case jdupgraded::dsp::ToneCouplingMode::ringPair01: couplingIndex = 1; break;
        case jdupgraded::dsp::ToneCouplingMode::ringPair23: couplingIndex = 2; break;
        case jdupgraded::dsp::ToneCouplingMode::crossModPair01: couplingIndex = 3; break;
        case jdupgraded::dsp::ToneCouplingMode::crossModPair23: couplingIndex = 4; break;
        case jdupgraded::dsp::ToneCouplingMode::hardSyncPair01: couplingIndex = 5; break;
        case jdupgraded::dsp::ToneCouplingMode::hardSyncPair23: couplingIndex = 6; break;
        default: break;
    }
    setApvtsChoice (kCouplingModeId, couplingIndex);

    const char* levelIds[4] = { kTone1LevelId, kTone2LevelId, kTone3LevelId, kTone4LevelId };
    const char* waveIds[4] = { kTone1WaveId, kTone2WaveId, kTone3WaveId, kTone4WaveId };
    const char* msIds[4] = { kTone1MultisampleId, kTone2MultisampleId, kTone3MultisampleId, kTone4MultisampleId };
    const char* muteIds[4] = { kTone1MuteId, kTone2MuteId, kTone3MuteId, kTone4MuteId };

    for (int t = 0; t < 4; ++t)
    {
        const auto& tone = patch.tones[static_cast<std::size_t> (t)];
        toneCoarseSemis_[static_cast<std::size_t> (t)] = tone.coarseSemis;

        setApvtsFloat (levelIds[t], tone.level);
        setApvtsFloat (jdupgraded::params::toneFilterParamId (t + 1, "Cutoff").toRawUTF8(), tone.filterCutoffNorm);
        setApvtsFloat (jdupgraded::params::toneFilterParamId (t + 1, "Resonance").toRawUTF8(),
                       tone.filterResonanceNorm);
        setApvtsInt (waveIds[t], static_cast<int> (tone.waveIndex));
        setApvtsInt (msIds[t], static_cast<int> (tone.multisampleSetId));
        setApvtsBool (muteIds[t], tone.level < 0.001f);
    }

    setApvtsFloat (kFilterResonanceId, patch.tones[0].filterResonanceNorm);
    copyGlobalFilterResonanceToAllTones();
    applyEnvelopeDefaultsForProgram (index);
    refreshCachedParameters();
    applyPatchesFromParameters();
}

void JDUpgradedAudioProcessor::applyPatchesFromParameters() noexcept
{
    jdupgraded::dsp::TonePatchSet patches{};

    const float levels[4] = {
        tone1LevelPtr_ != nullptr ? tone1LevelPtr_->load() : 1.0f,
        tone2LevelPtr_ != nullptr ? tone2LevelPtr_->load() : 0.0f,
        tone3LevelPtr_ != nullptr ? tone3LevelPtr_->load() : 0.0f,
        tone4LevelPtr_ != nullptr ? tone4LevelPtr_->load() : 0.0f,
    };

    const bool mutes[4] = {
        tone1MutePtr_ != nullptr && tone1MutePtr_->load() >= 0.5f,
        tone2MutePtr_ != nullptr && tone2MutePtr_->load() >= 0.5f,
        tone3MutePtr_ != nullptr && tone3MutePtr_->load() >= 0.5f,
        tone4MutePtr_ != nullptr && tone4MutePtr_->load() >= 0.5f,
    };

    int couplingIndex = 0;
    if (auto* couplingParam = dynamic_cast<juce::AudioParameterChoice*> (apvts_.getParameter (kCouplingModeId)))
        couplingIndex = couplingParam->getIndex();
    const auto coupling = couplingFromIndex (couplingIndex);

    voicePool_.setCouplingMode (coupling);

    const std::size_t waveIndices[4] = {
        tone1WavePtr_ != nullptr ? static_cast<std::size_t> (tone1WavePtr_->load()) : 0,
        tone2WavePtr_ != nullptr ? static_cast<std::size_t> (tone2WavePtr_->load()) : 16,
        tone3WavePtr_ != nullptr ? static_cast<std::size_t> (tone3WavePtr_->load()) : 32,
        tone4WavePtr_ != nullptr ? static_cast<std::size_t> (tone4WavePtr_->load()) : 48,
    };

    const std::uint16_t multisampleIds[4] = {
        tone1MultisamplePtr_ != nullptr ? static_cast<std::uint16_t> (tone1MultisamplePtr_->load()) : 0,
        tone2MultisamplePtr_ != nullptr ? static_cast<std::uint16_t> (tone2MultisamplePtr_->load()) : 0,
        tone3MultisamplePtr_ != nullptr ? static_cast<std::uint16_t> (tone3MultisamplePtr_->load()) : 0,
        tone4MultisamplePtr_ != nullptr ? static_cast<std::uint16_t> (tone4MultisamplePtr_->load()) : 0,
    };

    const jdupgraded::assets::RomBank* bank = romLoader_.getBank().isLoaded() ? &romLoader_.getBank() : nullptr;

    const float globalResonance = filterResonancePtr_ != nullptr ? filterResonancePtr_->load() : 0.35f;
    const bool filterLinked = isFilterLinked();
    const float ampA = ampAttackPtr_ != nullptr ? ampAttackPtr_->load() : 0.005f;
    const float ampD = ampDecayPtr_ != nullptr ? ampDecayPtr_->load() : 0.2f;
    const float ampS = ampSustainPtr_ != nullptr ? ampSustainPtr_->load() : 0.85f;
    const float ampR = ampReleasePtr_ != nullptr ? ampReleasePtr_->load() : 0.35f;
    const float fltA = filterAttackPtr_ != nullptr ? filterAttackPtr_->load() : 0.008f;
    const float fltD = filterDecayPtr_ != nullptr ? filterDecayPtr_->load() : 0.25f;
    const float fltS = filterSustainPtr_ != nullptr ? filterSustainPtr_->load() : 0.65f;
    const float fltR = filterReleasePtr_ != nullptr ? filterReleasePtr_->load() : 0.4f;
    const bool envelopeLinked = isEnvelopeLinked();

    for (std::size_t t = 0; t < jdupgraded::dsp::kTonesPerVoice; ++t)
    {
        const auto& env = toneEnvelopePtrs_[t];
        const float toneAmpA = envelopeLinked || env.ampAttack == nullptr ? ampA : env.ampAttack->load();
        const float toneAmpD = envelopeLinked || env.ampDecay == nullptr ? ampD : env.ampDecay->load();
        const float toneAmpS = envelopeLinked || env.ampSustain == nullptr ? ampS : env.ampSustain->load();
        const float toneAmpR = envelopeLinked || env.ampRelease == nullptr ? ampR : env.ampRelease->load();
        const float toneFltA = envelopeLinked || env.filterAttack == nullptr ? fltA : env.filterAttack->load();
        const float toneFltD = envelopeLinked || env.filterDecay == nullptr ? fltD : env.filterDecay->load();
        const float toneFltS = envelopeLinked || env.filterSustain == nullptr ? fltS : env.filterSustain->load();
        const float toneFltR = envelopeLinked || env.filterRelease == nullptr ? fltR : env.filterRelease->load();

        patches[t].romBank = bank;
        patches[t].multisampleSetId = multisampleIds[t];
        patches[t].waveIndex = static_cast<std::uint16_t> (waveIndices[t]);
        patches[t].coarseSemis = toneCoarseSemis_[t];
        patches[t].fineCents = toneFineCents_[t];
        const float cutoff = toneFilterCutoffPtrs_[t] != nullptr ? toneFilterCutoffPtrs_[t]->load() : 1.0f;
        const float resonance = filterLinked || toneFilterResonancePtrs_[t] == nullptr
                                    ? globalResonance
                                    : toneFilterResonancePtrs_[t]->load();
        patches[t].filterCutoffNorm = cutoff;
        patches[t].filterResonanceNorm = resonance;
        patches[t].level = mutes[t] ? 0.0f : levels[t];
        patches[t].phaseModDepth = 0.0f;
        patches[t].attackTimeSec = toneAmpA;
        patches[t].decayTimeSec = toneAmpD;
        patches[t].sustainLevel = toneAmpS;
        patches[t].releaseTimeSec = toneAmpR;
        patches[t].filterAttackTimeSec = toneFltA;
        patches[t].filterDecayTimeSec = toneFltD;
        patches[t].filterSustainLevel = toneFltS;
        patches[t].filterReleaseTimeSec = toneFltR;
        const auto& pitchMod = tonePitchMod_[t];
        patches[t].pitchLevel0Mult = pitchMod.pitchLevel0Mult;
        patches[t].pitchLevel1Mult = pitchMod.pitchLevel1Mult;
        patches[t].pitchLevel2Mult = pitchMod.pitchLevel2Mult;
        patches[t].pitchAttackTimeSec = pitchMod.pitchAttackSec;
        patches[t].pitchDecayTimeSec = pitchMod.pitchDecaySec;
        patches[t].pitchReleaseTimeSec = pitchMod.pitchReleaseSec;
        patches[t].lfo1RateHz = pitchMod.lfo1RateHz;
        patches[t].lfo1PitchDepthSemis = pitchMod.lfo1PitchDepthSemis;
        patches[t].waveform = bank == nullptr ? &fallbackWaves_.getWave (waveIndices[t]) : nullptr;
    }

    if (coupling == jdupgraded::dsp::ToneCouplingMode::crossModPair01)
        patches[1].phaseModDepth = 0.4f;
    if (coupling == jdupgraded::dsp::ToneCouplingMode::crossModPair23)
        patches[3].phaseModDepth = 0.4f;

    voicePool_.setDefaultPatches (patches);
}

void JDUpgradedAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    voicePool_.prepare (sampleRate, static_cast<std::size_t> (samplesPerBlock));
    groupB_.prepare (sampleRate);
    applyPatchesFromParameters();
}

void JDUpgradedAudioProcessor::releaseResources() {}

bool JDUpgradedAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

bool JDUpgradedAudioProcessor::applyJdSysexMessage (const std::uint8_t* data, std::size_t size) noexcept
{
    const auto result = jdupgraded::preset::ApvtsBridge::applyRolandSysEx (apvts_, data, size);
    if (! result.patchApplied)
        return false;

    std::size_t payloadBytes = 0;
    const auto* payload = jdupgraded::preset::JdPatchSysexMapper::findPatchPayload (data, size, payloadBytes);
    if (payload != nullptr)
        applyJdPatchCoarsePitch (payload, payloadBytes);

    refreshCachedParameters();
    applyPatchesFromParameters();
    return true;
}

void JDUpgradedAudioProcessor::applyJdPatchCoarsePitch (const std::uint8_t* patch,
                                                         std::size_t patchBytes) noexcept
{
    if (patch == nullptr || patchBytes < jdupgraded::preset::kJdPatchTotalBytes)
        return;

    for (std::size_t t = 0; t < jdupgraded::preset::kJdTonesPerPatch; ++t)
    {
        const auto base = jdupgraded::preset::kJdToneBlockOffset (t);
        const auto coarse = patch[base + jdupgraded::preset::kJdTonePitchCoarse];
        toneCoarseSemis_[t] = static_cast<float> (static_cast<int> (coarse & 0x7F) - 48);

        const auto fine = patch[base + jdupgraded::preset::kJdTonePitchFine];
        toneFineCents_[t] =
            static_cast<float> (static_cast<int> (fine & 0x7F) - jdupgraded::preset::kJdPitchLevelCenter);
        tonePitchMod_[t] = jdupgraded::preset::decodeTonePitchMod (patch, patchBytes, t);

        const int tone = static_cast<int> (t) + 1;
        const float cutoff =
            static_cast<float> (patch[base + jdupgraded::preset::kJdToneTvfCutoff] & 0x7F) / 127.0f;
        const float resonance =
            static_cast<float> (patch[base + jdupgraded::preset::kJdToneTvfResonance] & 0x7F) / 127.0f;
        setApvtsFloat (jdupgraded::params::toneFilterParamId (tone, "Cutoff").toRawUTF8(), cutoff);
        setApvtsFloat (jdupgraded::params::toneFilterParamId (tone, "Resonance").toRawUTF8(), resonance);
    }

    setApvtsBool (jdupgraded::params::kFilterLinkId, false);
}

void JDUpgradedAudioProcessor::handleMidi (const juce::MidiBuffer& midi) noexcept
{
    for (const auto metadata : midi)
    {
        const auto message = metadata.getMessage();
        if (message.isSysEx())
        {
            const auto* sysex = message.getSysExData();
            const auto sysexSize = static_cast<std::size_t> (message.getSysExDataSize());
            applyJdSysexMessage (reinterpret_cast<const std::uint8_t*> (sysex), sysexSize);
            continue;
        }

        if (message.isNoteOn())
        {
            voicePool_.noteOn (static_cast<std::uint8_t> (message.getNoteNumber()),
                               static_cast<std::uint8_t> (message.getVelocity()));
        }
        else if (message.isNoteOff())
        {
            voicePool_.noteOff (static_cast<std::uint8_t> (message.getNoteNumber()));
        }
        else if (message.isAllNotesOff() || message.isAllSoundOff())
        {
            voicePool_.allNotesOff();
        }
    }
}

void JDUpgradedAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    applyPatchesFromParameters();

    const auto totalNumInputChannels = getTotalNumInputChannels();
    for (int i = totalNumInputChannels; i < getTotalNumOutputChannels(); ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    handleMidi (midiMessages);

    const auto numSamples = static_cast<std::size_t> (buffer.getNumSamples());
    auto* left = buffer.getWritePointer (0);
    float* right = buffer.getNumChannels() > 1 ? buffer.getWritePointer (1) : nullptr;

    voicePool_.render (outputScratch_.data(), numSamples);

    const float master = masterGainPtr_ != nullptr ? masterGainPtr_->load() : 0.75f;
    const bool groupAOn = groupAEnablePtr_ == nullptr || groupAEnablePtr_->load() >= 0.5f;
    const bool groupBOn = groupBEnablePtr_ == nullptr || groupBEnablePtr_->load() >= 0.5f;
    const float groupADrive = groupAOn && groupADrivePtr_ != nullptr ? groupADrivePtr_->load() : 0.0f;
    const float groupBMix = groupBOn && groupBMixPtr_ != nullptr ? groupBMixPtr_->load() : 0.0f;
    const float groupBChorus = groupBOn && groupBChorusPtr_ != nullptr ? groupBChorusPtr_->load() : 0.0f;

    groupA_.setDrive (groupADrive);
    groupB_.setMix (groupBMix);
    groupB_.setChorus (groupBChorus);

    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        float l = outputScratch_[static_cast<std::size_t> (i)] * master;
        float r = l;

        l = groupA_.process (l);
        r = groupA_.process (r);

        groupB_.processStereo (l, r);

        left[i] = l;
        if (right != nullptr)
            right[i] = r;
    }
}

void JDUpgradedAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts_.copyState();
    state.setProperty ("currentProgram", currentProgram_, nullptr);
    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, destData);
}

void JDUpgradedAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
    {
        if (xml->hasTagName (apvts_.state.getType()))
        {
            auto state = juce::ValueTree::fromXml (*xml);
            apvts_.replaceState (state);
            currentProgram_ = static_cast<int> (state.getProperty ("currentProgram", 0));
        }
    }

    refreshCachedParameters();
    applyPatchesFromParameters();
}

juce::AudioProcessorEditor* JDUpgradedAudioProcessor::createEditor()
{
    return new JDUpgradedAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JDUpgradedAudioProcessor();
}
