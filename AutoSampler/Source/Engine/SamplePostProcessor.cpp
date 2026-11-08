#include "SamplePostProcessor.h"

#include <cmath>
#include <limits>
#include <vector>

namespace autosampler
{

juce::AudioBuffer<float> SamplePostProcessor::toMono (const juce::AudioBuffer<float>& src)
{
    juce::AudioBuffer<float> mono (1, src.getNumSamples());
    mono.clear();

    if (src.getNumSamples() == 0)
        return mono;

    const int channels = juce::jmax (1, src.getNumChannels());
    for (int i = 0; i < src.getNumSamples(); ++i)
    {
        float sum = 0.0f;
        for (int ch = 0; ch < channels; ++ch)
            sum += src.getSample (ch, i);
        mono.setSample (0, i, sum / static_cast<float> (channels));
    }

    return mono;
}

void SamplePostProcessor::trimSilence (Zone& zone, const PostProcessConfig& config)
{
    const auto mono = toMono (zone.audio);
    const int n = mono.getNumSamples();
    if (n < 8)
        return;

    const float thresh = juce::Decibels::decibelsToGain (config.trimThresholdDb);
    const int pad = static_cast<int> (std::ceil (config.trimPadMs * 0.001 * zone.sampleRate));

    int start = 0;
    for (; start < n; ++start)
    {
        if (std::abs (mono.getSample (0, start)) >= thresh)
            break;
    }

    int end = n - 1;
    for (; end > start; --end)
    {
        if (std::abs (mono.getSample (0, end)) >= thresh)
            break;
    }

    start = juce::jmax (0, start - pad);
    end = juce::jmin (n - 1, end + pad);

    if (end <= start)
        return;

    const int newLen = end - start + 1;
    juce::AudioBuffer<float> trimmed (zone.audio.getNumChannels(), newLen);

    for (int ch = 0; ch < zone.audio.getNumChannels(); ++ch)
        trimmed.copyFrom (ch, 0, zone.audio, ch, start, newLen);

    zone.audio = std::move (trimmed);
    zone.loopStart = -1;
    zone.loopEnd = -1;
}

void SamplePostProcessor::normalizePeak (Zone& zone, const PostProcessConfig& config)
{
    float peak = 0.0f;
    for (int ch = 0; ch < zone.audio.getNumChannels(); ++ch)
    {
        const float p = zone.audio.getMagnitude (ch, 0, zone.audio.getNumSamples());
        peak = juce::jmax (peak, p);
    }

    if (peak <= 1.0e-8f)
        return;

    const float target = juce::Decibels::decibelsToGain (config.normalizePeakDb);
    const float gain = target / peak;
    zone.audio.applyGain (gain);
}

void SamplePostProcessor::findZeroCrossLoop (Zone& zone, const PostProcessConfig& config)
{
    zone.loopStart = -1;
    zone.loopEnd = -1;

    const int n = zone.audio.getNumSamples();
    const int minLoop = static_cast<int> (std::ceil (config.minLoopSeconds * zone.sampleRate));
    if (n < minLoop * 2)
        return;

    const auto mono = toMono (zone.audio);
    const int window = juce::jmax (64, static_cast<int> (zone.sampleRate * 0.01));
    const int hop = window / 2;

    float peakRms = 0.0f;
    std::vector<float> rms;
    rms.reserve (static_cast<std::size_t> (n / hop + 1));

    for (int i = 0; i + window < n; i += hop)
    {
        float sum = 0.0f;
        for (int j = 0; j < window; ++j)
        {
            const float s = mono.getSample (0, i + j);
            sum += s * s;
        }
        const float r = std::sqrt (sum / static_cast<float> (window));
        rms.push_back (r);
        peakRms = juce::jmax (peakRms, r);
    }

    if (peakRms <= 1.0e-8f || rms.empty())
        return;

    const float sustainGate = peakRms * config.sustainLevelRatio;
    int firstSustainFrame = -1;
    int lastSustainFrame = static_cast<int> (rms.size()) - 1;

    for (int i = 0; i < static_cast<int> (rms.size()); ++i)
    {
        if (rms[static_cast<std::size_t> (i)] >= sustainGate)
        {
            firstSustainFrame = i;
            break;
        }
    }

    for (int i = static_cast<int> (rms.size()) - 1; i >= 0; --i)
    {
        if (rms[static_cast<std::size_t> (i)] >= sustainGate)
        {
            lastSustainFrame = i;
            break;
        }
    }

    if (firstSustainFrame < 0)
        return;

    const int searchStart = juce::jlimit (0, n - minLoop - 1, firstSustainFrame * hop);
    const int searchEnd = juce::jlimit (searchStart + minLoop, n - 1, lastSustainFrame * hop + window);

    std::vector<int> posCrossings;
    for (int i = searchStart + 1; i <= searchEnd; ++i)
    {
        const float a = mono.getSample (0, i - 1);
        const float b = mono.getSample (0, i);
        if (a <= 0.0f && b > 0.0f)
            posCrossings.push_back (i);
    }

    if (posCrossings.size() < 2)
        return;

    int bestStart = -1;
    int bestEnd = -1;
    float bestScore = std::numeric_limits<float>::max();

    for (int ei = static_cast<int> (posCrossings.size()) - 1; ei >= 1; --ei)
    {
        const int loopEnd = posCrossings[static_cast<std::size_t> (ei)];
        for (int si = ei - 1; si >= 0; --si)
        {
            const int loopStart = posCrossings[static_cast<std::size_t> (si)];
            if (loopEnd - loopStart < minLoop)
                continue;

            const float delta = std::abs (mono.getSample (0, loopStart) - mono.getSample (0, loopEnd));
            const float slopeStart = mono.getSample (0, loopStart + 1) - mono.getSample (0, loopStart);
            const float slopeEnd = mono.getSample (0, loopEnd + 1) - mono.getSample (0, loopEnd);
            const float slopeMismatch = std::abs (slopeStart - slopeEnd);
            const float score = delta + slopeMismatch;

            if (score < bestScore)
            {
                bestScore = score;
                bestStart = loopStart;
                bestEnd = loopEnd;
            }
            break;
        }
    }

    if (bestStart >= 0 && bestEnd > bestStart)
    {
        const int compareLen = juce::jmin (256, bestEnd - bestStart);
        float dot = 0.0f;
        float energyA = 0.0f;
        float energyB = 0.0f;

        for (int i = 0; i < compareLen; ++i)
        {
            const float a = mono.getSample (0, bestStart + i);
            const float b = mono.getSample (0, bestEnd + i);
            dot += a * b;
            energyA += a * a;
            energyB += b * b;
        }

        const float denom = std::sqrt (energyA * energyB) + 1.0e-8f;
        const float correlation = juce::jlimit (0.0f, 1.0f, dot / denom);
        const float slopeScore = 1.0f / (1.0f + bestScore);

        zone.loopConfidence = 0.6f * correlation + 0.4f * slopeScore;

        if (zone.loopConfidence >= config.minLoopConfidence)
        {
            zone.loopStart = bestStart;
            zone.loopEnd = bestEnd;
        }
        else
        {
            zone.loopStart = -1;
            zone.loopEnd = -1;
            zone.loopConfidence = correlation;
        }
    }
}

void SamplePostProcessor::processZone (Zone& zone, const PostProcessConfig& config)
{
    if (zone.audio.getNumSamples() == 0)
        return;

    if (config.trimSilence)
        trimSilence (zone, config);

    if (config.normalize)
        normalizePeak (zone, config);

    if (config.findSustainLoop)
        findZeroCrossLoop (zone, config);
}

} // namespace autosampler
