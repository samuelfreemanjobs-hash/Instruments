#include "FMEngine.h"

#include <algorithm>
#include <cmath>

namespace characterfm::dsp
{

void FMEngine::reset (double sampleRate) noexcept
{
    sampleRate_ = sampleRate;
    tick_ = 0;
    for (auto& slot : voices_)
    {
        slot.voice.reset (sampleRate);
        slot.note = -1;
        slot.age = 0;
        slot.keyDown = false;
    }
}

void FMEngine::allNotesOff() noexcept
{
    for (auto& slot : voices_)
    {
        slot.voice.stop();
        slot.keyDown = false;
    }
}

void FMEngine::setGlobalVoiceParams (const VoiceParams& params) noexcept
{
    globalParams_ = params;
    for (auto& slot : voices_)
        if (slot.voice.isActive() || slot.keyDown)
            slot.voice.setVoiceParams (globalParams_);
}

void FMEngine::setAlgorithmIndex (int index) noexcept
{
    globalParams_.algorithmIndex = index;
    for (auto& slot : voices_)
        slot.voice.setAlgorithmIndex (index);
}

float FMEngine::midiNoteToHz (int note) noexcept
{
    return 440.0f * std::pow (2.0f, (static_cast<float> (note) - 69.0f) / 12.0f);
}

int FMEngine::findFreeVoice() noexcept
{
    for (int i = 0; i < kMaxVoices; ++i)
        if (! voices_[static_cast<std::size_t> (i)].voice.isActive()
            && voices_[static_cast<std::size_t> (i)].note < 0)
            return i;
    return -1;
}

int FMEngine::findVoiceForNote (int note) noexcept
{
    for (int i = 0; i < kMaxVoices; ++i)
        if (voices_[static_cast<std::size_t> (i)].note == note)
            return i;
    return -1;
}

int FMEngine::stealVoice() noexcept
{
    int oldest = 0;
    std::uint32_t maxAge = 0;
    for (int i = 0; i < kMaxVoices; ++i)
    {
        const auto age = voices_[static_cast<std::size_t> (i)].age;
        if (age >= maxAge)
        {
            maxAge = age;
            oldest = i;
        }
    }
    voices_[static_cast<std::size_t> (oldest)].voice.stop();
    voices_[static_cast<std::size_t> (oldest)].note = -1;
    voices_[static_cast<std::size_t> (oldest)].keyDown = false;
    return oldest;
}

void FMEngine::noteOn (int noteNumber, float velocity) noexcept
{
    int idx = findVoiceForNote (noteNumber);
    if (idx < 0)
    {
        idx = findFreeVoice();
        if (idx < 0)
            idx = stealVoice();
    }

    auto& slot = voices_[static_cast<std::size_t> (idx)];
    slot.note = noteNumber;
    slot.keyDown = true;
    slot.age = tick_++;

    VoiceParams p = globalParams_;
    p.pitchHz = midiNoteToHz (noteNumber);
    slot.voice.start (velocity, p);
}

void FMEngine::noteOff (int noteNumber) noexcept
{
    const int idx = findVoiceForNote (noteNumber);
    if (idx < 0)
        return;

    auto& slot = voices_[static_cast<std::size_t> (idx)];
    slot.keyDown = false;
    slot.voice.stop();
    if (! slot.voice.isActive())
        slot.note = -1;
}

void FMEngine::renderBlock (float* left, float* right, int numSamples, float masterGain) noexcept
{
    for (int i = 0; i < numSamples; ++i)
    {
        float s = 0.0f;
        for (auto& slot : voices_)
            s += slot.voice.renderSample();

        s *= masterGain;
        left[i] = s;
        right[i] = s;
    }

    for (auto& slot : voices_)
    {
        if (! slot.keyDown && ! slot.voice.isActive())
            slot.note = -1;
    }
}

} // namespace characterfm::dsp
