#pragma once

#include "../Memory/TwelveBitBuffer.h"

namespace sp1200
{
class SampleVoice
{
public:
    void start (const TwelveBitBuffer* data,
                float velocity,
                float tuneSemitones,
                float level,
                float pan = 0.0f,
                std::size_t startSample = 0,
                std::size_t endSample = 0,
                float decayNorm = 1.0f,
                double hostSampleRate = 44100.0,
                int padIndex = -1);
    void forceStop() noexcept;
    [[nodiscard]] int padIndex() const noexcept { return padIndex_; }
    [[nodiscard]] bool isActive() const noexcept { return active_; }

    float renderNextSample() noexcept;
    [[nodiscard]] float lastPanLeft() const noexcept { return panL_; }
    [[nodiscard]] float lastPanRight() const noexcept { return panR_; }

private:
    const TwelveBitBuffer* data_ = nullptr;
    std::size_t startSample_ = 0;
    double endPhase_ = 0.0;
    double phase_ = 0.0;
    double phaseInc_ = 1.0;
    float level_ = 1.0f;
    float panL_ = 1.0f;
    float panR_ = 1.0f;
    bool active_ = false;
    int padIndex_ = -1;
    float ampEnv_ = 1.0f;
    float ampDecayCoeff_ = 1.0f;
};

} // namespace sp1200
