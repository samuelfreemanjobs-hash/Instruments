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
    const auto layers = zone.velocityLayersForExport();

    const VelocityLayer* layer = &layers.front();
    if (layers.size() > 1)
    {
        const int velMidi = static_cast<int> (v.gain * 127.0f);
        for (const auto& candidate : layers)
        {
            if (velMidi >= candidate.velStart && velMidi <= candidate.velEnd)
            {
                layer = &candidate;
                break;
            }
        }
    }

    if (layer->audio.getNumSamples() < 2)
        return 0.0f;

    const int loopStart = layer->hasLoop() ? layer->loopStart : -1;
    const int loopEnd = layer->hasLoop() ? layer->loopEnd : -1;

    double pos = v.position;
    if (loopStart >= 0 && loopEnd > loopStart && pos >= static_cast<double> (loopEnd))
    {
        const double loopLen = static_cast<double> (loopEnd - loopStart);
        pos = static_cast<double> (loopStart) + std::fmod (pos - static_cast<double> (loopStart), loopLen);
    }

    const int i0 = static_cast<int> (pos);
    const int i1 = i0 + 1;

    if (i0 >= layer->audio.getNumSamples())
        return 0.0f;

    const float frac = static_cast<float> (pos - static_cast<double> (i0));
    float sum = 0.0f;

    for (int ch = 0; ch < layer->audio.getNumChannels(); ++ch)
    {
        const float s0 = layer->audio.getSample (ch, i0);
        const float s1 = layer->audio.getSample (ch, juce::jmin (i1, layer->audio.getNumSamples() - 1));
        sum += s0 + frac * (s1 - s0);
    }

    if (loopStart >= 0 && loopEnd > loopStart && v.crossfadeRemaining > 0)
    {
        const float fade = static_cast<float> (v.crossfadeRemaining) / static_cast<float> (v.crossfadeTotal);
        const int cf = loopEnd - loopStart;
        if (cf > 0)
        {
            const int mirror = loopStart + (loopEnd - i0) % cf;
            if (mirror >= 0 && mirror < layer->audio.getNumSamples())
            {
                float tail = 0.0f;
                for (int ch = 0; ch < layer->audio.getNumChannels(); ++ch)
                    tail += layer->audio.getSample (ch, mirror);
                tail /= static_cast<float> (layer->audio.getNumChannels());
                sum = sum * (1.0f - fade) + tail * fade;
            }
        }
    }

    return sum / static_cast<float> (layer->audio.getNumChannels());
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
                const auto layers = zone.velocityLayersForExport();
                const VelocityLayer& layer = [&]
                {
                    if (layers.size() == 1)
                        return layers.front();
                    const int velMidi = static_cast<int> (v.gain * 127.0f);
                    for (const auto& candidate : layers)
                        if (velMidi >= candidate.velStart && velMidi <= candidate.velEnd)
                            return candidate;
                    return layers.front();
                }();

                const double ratio = std::pow (2.0, (v.note - zone.rootNote) / 12.0)
                                     * (zone.sampleRate / sampleRate_);
                const double prevPos = v.position;
                v.position += ratio;

                if (layer.hasLoop())
                {
                    const double loopLen = static_cast<double> (layer.loopEnd - layer.loopStart);
                    if (prevPos < static_cast<double> (layer.loopEnd) && v.position >= static_cast<double> (layer.loopEnd))
                    {
                        v.crossfadeTotal = 64;
                        v.crossfadeRemaining = 64;
                        const double overshoot = v.position - static_cast<double> (layer.loopStart);
                        v.position = static_cast<double> (layer.loopStart) + std::fmod (overshoot, loopLen);
                    }

                    if (v.crossfadeRemaining > 0)
                        --v.crossfadeRemaining;
                }
                else if (v.position >= layer.audio.getNumSamples())
                {
                    v.active = false;
                }
            }
        }

        sample *= gain;

        for (int ch = 0; ch < numCh; ++ch)
            buffer.setSample (ch, i, sample);
    }
}

} // namespace autosampler
