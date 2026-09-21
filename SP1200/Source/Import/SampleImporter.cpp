#include "SampleImporter.h"

#include "../SP1200Constants.h"

namespace sp1200
{
namespace
{
void monoSumBuffer (juce::AudioBuffer<float>& buffer)
{
    if (buffer.getNumChannels() <= 1)
        return;

    auto* mono = buffer.getWritePointer (0);
    const int n = buffer.getNumSamples();
    for (int ch = 1; ch < buffer.getNumChannels(); ++ch)
    {
        const auto* src = buffer.getReadPointer (ch);
        for (int i = 0; i < n; ++i)
            mono[i] += src[i];
    }
    const float inv = 1.0f / static_cast<float> (buffer.getNumChannels());
    for (int i = 0; i < n; ++i)
        mono[i] *= inv;
}

std::vector<float> resampleToSpRate (const float* src, int numSrc, double sourceRate)
{
    if (numSrc <= 0)
        return {};

    const double outRate = kSampleRateHz;
    const int outLen = static_cast<int> (std::ceil (static_cast<double> (numSrc) * outRate / sourceRate));
    std::vector<float> out (static_cast<std::size_t> (std::max (outLen, 1)));

    for (int i = 0; i < outLen; ++i)
    {
        const double srcPos = static_cast<double> (i) * sourceRate / outRate;
        const int i0 = static_cast<int> (srcPos);
        const int i1 = std::min (i0 + 1, numSrc - 1);
        const float frac = static_cast<float> (srcPos - static_cast<double> (i0));
        const float s0 = src[static_cast<std::size_t> (std::clamp (i0, 0, numSrc - 1))];
        const float s1 = src[static_cast<std::size_t> (i1)];
        out[static_cast<std::size_t> (i)] = s0 + frac * (s1 - s0);
    }
    return out;
}
} // namespace

SampleImporter::SampleImporter()
{
    formatManager_.registerBasicFormats();
}

ImportResult SampleImporter::importFromFile (const juce::File& file,
                                             int bankIndex,
                                             juce::String segmentName,
                                             bool vinylRateTrick)
{
    ImportResult result;
    std::unique_ptr<juce::AudioFormatReader> reader (formatManager_.createReaderFor (file));
    if (reader == nullptr)
    {
        result.error = "Unsupported or missing audio file";
        return result;
    }

    juce::AudioBuffer<float> buffer (static_cast<int> (reader->numChannels),
                                     static_cast<int> (reader->lengthInSamples));
    reader->read (&buffer, 0, static_cast<int> (reader->lengthInSamples), 0, true, true);

    return importFromAudioBuffer (buffer, reader->sampleRate, bankIndex, std::move (segmentName), vinylRateTrick);
}

ImportResult SampleImporter::importFromAudioBuffer (const juce::AudioBuffer<float>& buffer,
                                                    double sourceSampleRate,
                                                    int bankIndex,
                                                    juce::String segmentName,
                                                    bool vinylRateTrick)
{
    ImportResult result;
    if (buffer.getNumSamples() <= 0 || sourceSampleRate <= 0.0)
    {
        result.error = "Empty buffer";
        return result;
    }

    juce::AudioBuffer<float> mono (1, buffer.getNumSamples());
    mono.copyFrom (0, 0, buffer, 0, 0, buffer.getNumSamples());
    if (buffer.getNumChannels() > 1)
    {
        juce::AudioBuffer<float> copy (buffer);
        monoSumBuffer (copy);
        mono.copyFrom (0, 0, copy, 0, 0, copy.getNumSamples());
    }

    if (vinylRateTrick)
        sourceSampleRate *= kVinylImportRateMultiplier;

    const auto resampled = resampleToSpRate (mono.getReadPointer (0), mono.getNumSamples(), sourceSampleRate);
    if (resampled.empty())
    {
        result.error = "Resample failed";
        return result;
    }

    if (static_cast<std::int64_t> (resampled.size()) > kMaxMemorySamples)
    {
        result.error = "Sample exceeds 7:00 TOTAL memory cap";
        return result;
    }

    SampleSegment seg;
    seg.name = segmentName.isNotEmpty() ? segmentName.toStdString() : "SAMPLE";
    seg.bank = std::clamp (bankIndex, 0, kNumBanks - 1);
    seg.data.quantizeFromFloat (resampled.data(), resampled.size());
    result.segment = std::move (seg);
    result.ok = true;
    return result;
}

} // namespace sp1200
