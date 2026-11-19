#include "PreviewSynth.h"

#include <cmath>

namespace disklordz::chordengine
{

void PreviewSynth::prepare(double sampleRate)
{
    sampleRate_ = sampleRate > 0.0 ? sampleRate : 44100.0;
    reset();
}

void PreviewSynth::reset()
{
    for (auto& v : voices_)
        v = {};
}

void PreviewSynth::noteOn(int midiNote, float velocity)
{
    const int idx = allocateVoice(midiNote);
  auto& v = voices_[static_cast<size_t>(idx)];
    v.active = true;
    v.note = midiNote;
    const double hz = juce::MidiMessage::getMidiNoteInHertz(midiNote);
    v.phaseInc = static_cast<float>(hz / sampleRate_);
    v.targetAmp = juce::jlimit(0.0f, 1.0f, velocity);
    v.amp = v.targetAmp;
    v.phase = 0.0f;
}

void PreviewSynth::noteOff(int midiNote)
{
    releaseVoice(midiNote);
}

void PreviewSynth::allNotesOff()
{
    for (auto& v : voices_)
    {
        if (v.active)
            v.targetAmp = 0.0f;
    }
}

int PreviewSynth::allocateVoice(int midiNote)
{
    for (size_t i = 0; i < voices_.size(); ++i)
        if (!voices_[i].active)
            return static_cast<int>(i);

    for (size_t i = 0; i < voices_.size(); ++i)
        if (voices_[i].note == midiNote)
            return static_cast<int>(i);

    return 0;
}

void PreviewSynth::releaseVoice(int midiNote)
{
    for (auto& v : voices_)
    {
        if (v.active && v.note == midiNote)
            v.targetAmp = 0.0f;
    }
}

void PreviewSynth::render(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    if (numChannels == 0 || numSamples == 0)
        return;

    buffer.clear();

    const float attack = 0.004f;
    const float release = 0.08f;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float out = 0.0f;
        for (auto& v : voices_)
        {
            if (!v.active && v.amp < 1.0e-5f)
                continue;

            const float coef = v.targetAmp > v.amp ? attack : release;
            v.amp += (v.targetAmp - v.amp) * coef;

            if (v.amp < 1.0e-5f && v.targetAmp < 1.0e-5f)
            {
                v.active = false;
                continue;
            }

            const float saw = 2.0f * v.phase - 1.0f;
            const float sine = std::sin(v.phase * juce::MathConstants<float>::twoPi);
            const float tone = 0.65f * saw + 0.35f * sine;
            out += tone * v.amp * 0.15f;

            v.phase += v.phaseInc;
            if (v.phase >= 1.0f)
                v.phase -= 1.0f;
        }

        for (int ch = 0; ch < numChannels; ++ch)
            buffer.setSample(ch, sample, out);
    }
}

} // namespace disklordz::chordengine