#include "SynthEngine.h"

namespace junovax::dsp
{
void SynthEngine::prepare (double sampleRate, int maxBlockSize) noexcept
{
    juce::ignoreUnused (maxBlockSize);
    sampleRate_ = sampleRate;
    diagTone_.prepare (sampleRate);
    masterGain_.reset (sampleRate, 0.02);
    masterGain_.setCurrentAndTargetValue (1.0f);
    reset();
}

void SynthEngine::reset() noexcept
{
    diagTone_.reset();
    voiceActive_ = false;
    voiceLevel_ = 0.0f;
}

void SynthEngine::panic() noexcept
{
    voiceActive_ = false;
    voiceLevel_ = 0.0f;
    diagTone_.setEnabled (false);
}

void SynthEngine::handleMidi (const juce::MidiBuffer& midi) noexcept
{
    for (const auto metadata : midi)
    {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn())
        {
            voiceActive_ = true;
            voiceLevel_ = msg.getFloatVelocity();
        }
        else if (msg.isNoteOff() || msg.isAllNotesOff() || msg.isAllSoundOff())
        {
            voiceActive_ = false;
            voiceLevel_ = 0.0f;
        }
    }
}

float SynthEngine::renderSample() noexcept
{
    diagTone_.setEnabled (params_.diagTestTone);
    diagTone_.setFrequencyHz (params_.diagToneFreqHz);

    if (params_.diagTestTone)
        return diagTone_.processSample();

    if (! voiceActive_)
        return 0.0f;

    juce::ignoreUnused (params_.filterCutoff, params_.filterRes, params_.hpfEnabled, params_.chorusMode);
    return voiceLevel_ * 0.08f;
}

void SynthEngine::render (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) noexcept
{
    handleMidi (midi);

    const float gain = juce::Decibels::decibelsToGain (params_.masterGainDb);
    masterGain_.setTargetValue (gain);

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer (ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            const float sample = renderSample() * masterGain_.getNextValue();
            data[i] = sample;
        }
    }
}
} // namespace junovax::dsp
