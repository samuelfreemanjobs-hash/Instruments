#pragma once

#include "FMVoice.h"

#include <array>
#include <cstdint>

namespace characterfm::dsp
{

constexpr int kMaxVoices = 24;

class FMEngine
{
public:
    void reset (double sampleRate) noexcept;
    void allNotesOff() noexcept;

    void setGlobalVoiceParams (const VoiceParams& params) noexcept;
    void setAlgorithmIndex (int index) noexcept;

    void noteOn (int noteNumber, float velocity) noexcept;
    void noteOff (int noteNumber) noexcept;

    void renderBlock (float* left, float* right, int numSamples, float masterGain) noexcept;

private:
    struct VoiceSlot
    {
        FMVoice voice;
        int note = -1;
        std::uint32_t age = 0;
        bool keyDown = false;
    };

    double sampleRate_ = 44100.0;
    VoiceParams globalParams_{};
    std::array<VoiceSlot, kMaxVoices> voices_{};
    std::uint32_t tick_ = 0;

    int findFreeVoice() noexcept;
    int findVoiceForNote (int note) noexcept;
    int stealVoice() noexcept;
    static float midiNoteToHz (int note) noexcept;
};

} // namespace characterfm::dsp
