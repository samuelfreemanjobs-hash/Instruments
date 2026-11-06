#include "SamplerEngine.h"

namespace autosampler
{

void SamplerEngine::prepare (double sampleRate, int maxBlockSize)
{
    sampleRate_ = sampleRate;
    juce::ignoreUnused (maxBlockSize);
    reset();
}

void SamplerEngine::reset() noexcept
{
    for (auto& v : voices_)
        v = {};
}

void SamplerEngine::startVoice (int note, float velocity)
{
    if (program_ == nullptr || program_->empty())
        return;

    const int zi = program_->zoneIndexForNote (note);
    if (zi < 0)
        return;

    for (auto& v : voices_)
    {
        if (v.active && v.note == note)
        {
            v.gain = velocity;
            return;
        }
    }

    for (auto& v : voices_)
    {
        if (! v.active)
        {
            v.active = true;
            v.zoneIndex = zi;
            v.note = note;
            v.gain = velocity;
            v.position = 0.0;
            return;
        }
    }
}

void SamplerEngine::stopVoice (int note)
{
    for (auto& v : voices_)
    {
        if (v.active && v.note == note)
            v.active = false;
    }
}

float SamplerEngine::renderSample (const Voice& v) const noexcept
{
    if (program_ == nullptr || v.zoneIndex < 0)
        return 0.0f;

    const auto& zones = program_->zones();
    if (v.zoneIndex >= static_cast<int> (zones.size()))
        return 0.0f;

    const auto& zone = zones[static_cast<std::size_t> (v.zoneIndex)];
    if (zone.audio.getNumSamples() < 2)
        return 0.0f;

    const double pos = v.position;
    const int i0 = static_cast<int> (pos);
    const int i1 = i0 + 1;

    if (i0 >= zone.audio.getNumSamples())
        return 0.0f;

    const float frac = static_cast<float> (pos - static_cast<double> (i0));
    float sum = 0.0f;

    for (int ch = 0; ch < zone.audio.getNumChannels(); ++ch)
    {
        const float s0 = zone.audio.getSample (ch, i0);
        const float s1 = zone.audio.getSample (ch, juce::jmin (i1, zone.audio.getNumSamples() - 1));
        sum += s0 + frac * (s1 - s0);
    }

    return sum / static_cast<float> (zone.audio.getNumChannels());
}

void SamplerEngine::process (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    buffer.clear();

    for (const auto metadata : midi)
    {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn())
            startVoice (msg.getNoteNumber(), msg.getFloatVelocity());
        else if (msg.isNoteOff())
            stopVoice (msg.getNoteNumber());
    }

    const float gain = juce::Decibels::decibelsToGain (outputGainDb_);
    const int numSamples = buffer.getNumSamples();
    const int numCh = buffer.getNumChannels();

    for (int i = 0; i < numSamples; ++i)
    {
        float sample = 0.0f;

        for (auto& v : voices_)
        {
            if (! v.active)
                continue;

            sample += renderSample (v) * v.gain;

            if (program_ != nullptr && v.zoneIndex >= 0)
            {
                const auto& zone = program_->zones()[static_cast<std::size_t> (v.zoneIndex)];
                const double ratio = std::pow (2.0, (v.note - zone.rootNote) / 12.0)
                                     * (zone.sampleRate / sampleRate_);
                v.position += ratio;

                if (v.position >= zone.audio.getNumSamples())
                    v.active = false;
            }
        }

        sample *= gain;

        for (int ch = 0; ch < numCh; ++ch)
            buffer.setSample (ch, i, sample);
    }
}

} // namespace autosampler
