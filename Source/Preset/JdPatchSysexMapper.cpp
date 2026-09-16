#include "JdPatchSysexMapper.h"

#include "JdEnvelopeScale.h"
#include "JdPatchLayout.h"
#include "Parameters/EnvelopeParameters.h"
#include "Parameters/EffectParameters.h"
#include "Parameters/FilterParameters.h"
#include "SysExParser.h"

#include <algorithm>
#include <cmath>

namespace jdupgraded::preset
{

namespace
{

float norm7 (std::uint8_t v) noexcept
{
    return static_cast<float> (v & 0x7F) / 127.0f;
}

int jdWaveToCleanroom (std::uint8_t msb, std::uint8_t lsb) noexcept
{
    const int jdIndex = (static_cast<int> (msb & 0x7F) << 7) | static_cast<int> (lsb & 0x7F);
    constexpr int kJdWaveCount = 108;
    const int clamped = std::clamp (jdIndex, 0, kJdWaveCount - 1);
    return (clamped * 255) / (kJdWaveCount - 1);
}

bool toneEnabledInLayerMask (std::uint8_t layerMask, std::size_t toneIndex) noexcept
{
    const std::uint8_t bit = static_cast<std::uint8_t> (1u << toneIndex);
    return (layerMask & bit) != 0;
}

void setApvtsFloat (juce::AudioProcessorValueTreeState& apvts, const char* id, float value)
{
    if (auto* param = apvts.getParameter (id))
        param->setValueNotifyingHost (param->convertTo0to1 (value));
}

void setApvtsInt (juce::AudioProcessorValueTreeState& apvts, const char* id, int value)
{
    if (auto* param = apvts.getParameter (id))
        param->setValueNotifyingHost (param->convertTo0to1 (static_cast<float> (value)));
}

void setApvtsBool (juce::AudioProcessorValueTreeState& apvts, const char* id, bool value)
{
    if (auto* param = apvts.getParameter (id))
        param->setValueNotifyingHost (value ? 1.0f : 0.0f);
}

} // namespace

bool JdPatchSysexMapper::applyRawPatchBlock (juce::AudioProcessorValueTreeState& apvts,
                                             const std::uint8_t* patch,
                                             std::size_t patchBytes) noexcept
{
    if (patch == nullptr || patchBytes < kJdPatchTotalBytes)
        return false;

    const float master = norm7 (patch[kJdCommonPatchLevel]);
    setApvtsFloat (apvts, "masterGain", master);

    const std::uint8_t layerMask = patch[kJdCommonLayerToneMask];
    const auto effectBase = kJdPatchCommonBytes;
    const float groupADrive = norm7 (patch[effectBase + kJdEffectDistortionDrive]);
    const float groupBMix = norm7 (patch[effectBase + kJdEffectReverbLevel]);
    const float groupBChorus = norm7 (patch[effectBase + kJdEffectChorusLevel]);
    const float groupAPhaser = norm7 (patch[effectBase + kJdEffectPhaserMix]);
    const float groupBDelay = norm7 (patch[effectBase + kJdEffectDelayLeftLevel]);
    const bool groupAOn = (patch[effectBase + kJdEffectGroupABlock1Sw] & 0x7F) != 0;
    const bool groupBOn = (patch[effectBase + kJdEffectGroupBBlock3Sw] & 0x7F) != 0;
    setApvtsFloat (apvts, "groupADrive", groupADrive);
    setApvtsFloat (apvts, "groupBMix", groupBMix);
    setApvtsFloat (apvts, jdupgraded::params::kGroupBChorusId, groupBChorus);
    setApvtsFloat (apvts, jdupgraded::params::kGroupAPhaserId, groupAPhaser);
    setApvtsFloat (apvts, jdupgraded::params::kGroupBDelayId, groupBDelay);
    setApvtsBool (apvts, jdupgraded::params::kGroupAEnableId, groupAOn);
    setApvtsBool (apvts, jdupgraded::params::kGroupBEnableId, groupBOn);

    const char* levelIds[4] = { "tone1Level", "tone2Level", "tone3Level", "tone4Level" };
    const char* waveIds[4] = { "tone1Wave", "tone2Wave", "tone3Wave", "tone4Wave" };
    const char* muteIds[4] = { "tone1Mute", "tone2Mute", "tone3Mute", "tone4Mute" };

    float maxResonance = 0.0f;

    for (std::size_t t = 0; t < kJdTonesPerPatch; ++t)
    {
        const auto base = kJdToneBlockOffset (t);
        const int wave = jdWaveToCleanroom (patch[base + kJdToneWaveMsb], patch[base + kJdToneWaveLsb]);
        const float level = norm7 (patch[base + kJdToneTvaLevel]);
        const float resonance = norm7 (patch[base + kJdToneTvfResonance]);
        const float cutoff = norm7 (patch[base + kJdToneTvfCutoff]);
        maxResonance = std::max (maxResonance, resonance);

        const int tone = static_cast<int> (t) + 1;
        setApvtsFloat (apvts,
                       jdupgraded::params::toneFilterParamId (tone, "Cutoff").toRawUTF8(),
                       cutoff);
        setApvtsFloat (apvts,
                       jdupgraded::params::toneFilterParamId (tone, "Resonance").toRawUTF8(),
                       resonance);

        setApvtsInt (apvts, waveIds[t], wave);
        setApvtsFloat (apvts, levelIds[t], level);
        setApvtsBool (apvts, muteIds[t], ! toneEnabledInLayerMask (layerMask, t));
    }

    setApvtsFloat (apvts, "filterResonance", maxResonance);

    for (std::size_t t = 0; t < kJdTonesPerPatch; ++t)
    {
        const auto base = kJdToneBlockOffset (t);
        const int tone = static_cast<int> (t) + 1;

        const juce::String ampAttackId = jdupgraded::params::toneEnvelopeParamId (tone, "AmpAttack");
        const juce::String ampDecayId = jdupgraded::params::toneEnvelopeParamId (tone, "AmpDecay");
        const juce::String ampSustainId = jdupgraded::params::toneEnvelopeParamId (tone, "AmpSustain");
        const juce::String ampReleaseId = jdupgraded::params::toneEnvelopeParamId (tone, "AmpRelease");
        const juce::String filterAttackId = jdupgraded::params::toneEnvelopeParamId (tone, "FilterAttack");
        const juce::String filterDecayId = jdupgraded::params::toneEnvelopeParamId (tone, "FilterDecay");
        const juce::String filterSustainId = jdupgraded::params::toneEnvelopeParamId (tone, "FilterSustain");
        const juce::String filterReleaseId = jdupgraded::params::toneEnvelopeParamId (tone, "FilterRelease");

        setApvtsFloat (apvts, ampAttackId.toRawUTF8(),
                       jdEnvelopeTimeToSeconds (patch[base + kJdToneTvaEnvTime1]));
        setApvtsFloat (apvts, ampDecayId.toRawUTF8(),
                       jdEnvelopeTimeToSeconds (patch[base + kJdToneTvaEnvTime2]));
        setApvtsFloat (apvts, ampSustainId.toRawUTF8(),
                       jdEnvelopeLevelToNorm (patch[base + kJdToneTvaEnvSustain]));
        setApvtsFloat (apvts, ampReleaseId.toRawUTF8(),
                       jdEnvelopeTimeToSeconds (patch[base + kJdToneTvaEnvTime4]));

        setApvtsFloat (apvts, filterAttackId.toRawUTF8(),
                       jdEnvelopeTimeToSeconds (patch[base + kJdToneTvfEnvTime1]));
        setApvtsFloat (apvts, filterDecayId.toRawUTF8(),
                       jdEnvelopeTimeToSeconds (patch[base + kJdToneTvfEnvTime2]));
        setApvtsFloat (apvts, filterSustainId.toRawUTF8(),
                       jdEnvelopeLevelToNorm (patch[base + kJdToneTvfEnvSustain]));
        setApvtsFloat (apvts, filterReleaseId.toRawUTF8(),
                       jdEnvelopeTimeToSeconds (patch[base + kJdToneTvfEnvTime4]));
    }

    const auto tone0 = kJdToneBlockOffset (0);
    setApvtsFloat (apvts, "ampAttack",
                   jdEnvelopeTimeToSeconds (patch[tone0 + kJdToneTvaEnvTime1]));
    setApvtsFloat (apvts, "ampDecay",
                   jdEnvelopeTimeToSeconds (patch[tone0 + kJdToneTvaEnvTime2]));
    setApvtsFloat (apvts, "ampSustain",
                   jdEnvelopeLevelToNorm (patch[tone0 + kJdToneTvaEnvSustain]));
    setApvtsFloat (apvts, "ampRelease",
                   jdEnvelopeTimeToSeconds (patch[tone0 + kJdToneTvaEnvTime4]));
    setApvtsFloat (apvts, "filterAttack",
                   jdEnvelopeTimeToSeconds (patch[tone0 + kJdToneTvfEnvTime1]));
    setApvtsFloat (apvts, "filterDecay",
                   jdEnvelopeTimeToSeconds (patch[tone0 + kJdToneTvfEnvTime2]));
    setApvtsFloat (apvts, "filterSustain",
                   jdEnvelopeLevelToNorm (patch[tone0 + kJdToneTvfEnvSustain]));
    setApvtsFloat (apvts, "filterRelease",
                   jdEnvelopeTimeToSeconds (patch[tone0 + kJdToneTvfEnvTime4]));
    setApvtsBool (apvts, jdupgraded::params::kEnvelopeLinkId, false);
    setApvtsBool (apvts, jdupgraded::params::kFilterLinkId, false);

    return true;
}

const std::uint8_t* JdPatchSysexMapper::findPatchPayload (const std::uint8_t* data,
                                                          std::size_t size,
                                                          std::size_t& outPayloadBytes) noexcept
{
    outPayloadBytes = 0;
    if (data == nullptr || size < kJdPatchTotalBytes)
        return nullptr;

    if (size == kJdPatchTotalBytes)
    {
        outPayloadBytes = kJdPatchTotalBytes;
        return data;
    }

    if (size >= kJdPatchTotalBytes + 2 && data[0] == 0xF0)
    {
        const auto* tail = data + (size - kJdPatchTotalBytes - 2);
        if (tail[0] != 0xF7)
        {
            const auto* candidate = data + size - kJdPatchTotalBytes - 1;
            if (candidate > data && candidate[-1] != 0xF7)
            {
                outPayloadBytes = kJdPatchTotalBytes;
                return candidate;
            }
        }
    }

    for (std::size_t i = 0; i + kJdPatchTotalBytes <= size; ++i)
    {
        if (data[i] != 0xF0)
            continue;

        if (i + 9 >= size)
            continue;

        if (data[i + 1] != kRolandManufacturer)
            continue;

        const auto model = data[i + 3];
        if (model != kJd800ModelId && model != kJd990ModelId)
            continue;

        const auto cmd = data[i + 4];
        if (cmd != kRolandCmdDt1 && cmd != kRolandCmdDataTransfer && cmd != kRolandCmdDataTransferOneWay)
            continue;

        std::size_t dataStart = i + 9;
        if (cmd == kRolandCmdDataTransfer || cmd == kRolandCmdDataTransferOneWay)
            dataStart = i + 13;

        if (dataStart + kJdPatchTotalBytes <= size)
        {
            outPayloadBytes = kJdPatchTotalBytes;
            return data + dataStart;
        }
    }

    return nullptr;
}

JdPatchSysexResult JdPatchSysexMapper::applyToApvts (juce::AudioProcessorValueTreeState& apvts,
                                                     const std::uint8_t* data,
                                                     std::size_t size) noexcept
{
    JdPatchSysexResult result;

    if (data == nullptr || size < 8)
    {
        result.status = "empty";
        return result;
    }

    if (data[0] == 0xF0)
    {
        if (const auto parsed = SysExParser::feed (data, size))
            result.checksumValid = parsed->checksumValid;
    }
    else if (size >= kJdPatchTotalBytes)
    {
        result.patchApplied = applyRawPatchBlock (apvts, data, size);
        result.status = result.patchApplied ? "jd patch applied" : "patch decode failed";
        return result;
    }

    std::size_t payloadBytes = 0;
    const auto* payload = findPatchPayload (data, size, payloadBytes);

    if (payload == nullptr)
    {
        result.status = result.checksumValid ? "checksum ok, no patch block" : "unrecognized";
        return result;
    }

    result.patchApplied = applyRawPatchBlock (apvts, payload, payloadBytes);
    result.status = result.patchApplied ? "jd patch applied" : "patch decode failed";
    return result;
}

} // namespace jdupgraded::preset
