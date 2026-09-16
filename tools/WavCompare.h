#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

#include <cmath>
#include <string>

namespace jdupgraded::tools
{

struct WavCompareResult final
{
    bool ok = false;
    std::string error;
    double sampleRate = 0.0;
    int numSamples = 0;
    float rmsDiffDb = 0.0f;
    float spectralMeanDiffDb = 0.0f;
};

inline bool loadWavMono (const juce::File& file, juce::AudioBuffer<float>& mono, double& sampleRateOut)
{
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor (file));
    if (reader == nullptr)
        return false;

    sampleRateOut = reader->sampleRate;
    const int numSamples = static_cast<int> (reader->lengthInSamples);
    juce::AudioBuffer<float> interleaved (static_cast<int> (reader->numChannels), numSamples);
    reader->read (&interleaved, 0, numSamples, 0, true, true);

    mono.setSize (1, numSamples);
    mono.clear();
    for (int ch = 0; ch < interleaved.getNumChannels(); ++ch)
        mono.addFrom (0, 0, interleaved, ch, 0, numSamples, 1.0f / static_cast<float> (interleaved.getNumChannels()));

    return true;
}

inline void peakNormalize (juce::AudioBuffer<float>& buffer, float targetPeak = 0.95f)
{
    const float peak = buffer.getMagnitude (0, buffer.getNumSamples());
    if (peak > 1.0e-8f)
        buffer.applyGain (targetPeak / peak);
}

inline float computeRmsDiffDb (const juce::AudioBuffer<float>& a, const juce::AudioBuffer<float>& b)
{
    const int n = std::min (a.getNumSamples(), b.getNumSamples());
    if (n <= 0)
        return 0.0f;

    double sumSq = 0.0;
    for (int i = 0; i < n; ++i)
    {
        const double d = static_cast<double> (a.getSample (0, i) - b.getSample (0, i));
        sumSq += d * d;
    }

    const double rms = std::sqrt (sumSq / static_cast<double> (n));
    return static_cast<float> (20.0 * std::log10 (std::max (rms, 1.0e-12)));
}

inline float computeSpectralMeanDiffDb (const juce::AudioBuffer<float>& a, const juce::AudioBuffer<float>& b)
{
    constexpr int fftOrder = 11;
    constexpr int fftSize = 1 << fftOrder;
    constexpr int hop = 512;

    juce::dsp::FFT fft (fftOrder);
    std::vector<float> window (static_cast<std::size_t> (fftSize));
    for (int i = 0; i < fftSize; ++i)
    {
        const float x = static_cast<float> (i) / static_cast<float> (fftSize - 1);
        window[static_cast<std::size_t> (i)] = 0.5f - 0.5f * std::cos (2.0f * juce::MathConstants<float>::pi * x);
    }

    std::vector<float> fa (static_cast<std::size_t> (fftSize * 2), 0.0f);
    std::vector<float> fb (static_cast<std::size_t> (fftSize * 2), 0.0f);
    std::vector<float> magA (static_cast<std::size_t> (fftSize / 2 + 1), 0.0f);
    std::vector<float> magB (static_cast<std::size_t> (fftSize / 2 + 1), 0.0f);

    const int n = std::min (a.getNumSamples(), b.getNumSamples());
    if (n < fftSize)
        return computeRmsDiffDb (a, b);

    double sumDiff = 0.0;
    int frames = 0;

    for (int start = 0; start + fftSize <= n; start += hop)
    {
        std::fill (fa.begin(), fa.end(), 0.0f);
        std::fill (fb.begin(), fb.end(), 0.0f);

        for (int i = 0; i < fftSize; ++i)
        {
            fa[static_cast<std::size_t> (i)] = a.getSample (0, start + i) * window[static_cast<std::size_t> (i)];
            fb[static_cast<std::size_t> (i)] = b.getSample (0, start + i) * window[static_cast<std::size_t> (i)];
        }

        fft.performFrequencyOnlyForwardTransform (fa.data());
        fft.performFrequencyOnlyForwardTransform (fb.data());

        const int bins = fftSize / 2 + 1;
        for (int bin = 1; bin < bins; ++bin)
        {
            const float ma = fa[static_cast<std::size_t> (bin)];
            const float mb = fb[static_cast<std::size_t> (bin)];
            const float la = 20.0f * std::log10 (std::max (ma, 1.0e-12f));
            const float lb = 20.0f * std::log10 (std::max (mb, 1.0e-12f));
            sumDiff += static_cast<double> (std::abs (la - lb));
        }

        ++frames;
    }

    if (frames == 0)
        return 0.0f;

    return static_cast<float> (sumDiff / static_cast<double> (frames * (fftSize / 2)));
}

inline WavCompareResult compareWavFiles (const juce::File& fileA, const juce::File& fileB)
{
    WavCompareResult result;
    juce::AudioBuffer<float> a, b;
    double srA = 0.0, srB = 0.0;

    if (! loadWavMono (fileA, a, srA))
    {
        result.error = "Failed to read A";
        return result;
    }

    if (! loadWavMono (fileB, b, srB))
    {
        result.error = "Failed to read B";
        return result;
    }

    if (std::abs (srA - srB) > 1.0)
    {
        result.error = "Sample rate mismatch";
        return result;
    }

    peakNormalize (a);
    peakNormalize (b);

    result.sampleRate = srA;
    result.numSamples = std::min (a.getNumSamples(), b.getNumSamples());
    result.rmsDiffDb = computeRmsDiffDb (a, b);
    result.spectralMeanDiffDb = computeSpectralMeanDiffDb (a, b);
    result.ok = true;
    return result;
}

} // namespace jdupgraded::tools
