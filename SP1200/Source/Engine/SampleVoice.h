#pragma once

#include "../Memory/TwelveBitBuffer.h"

namespace sp1200
{
class SampleVoice
{
public:
    void start (const TwelveBitBuffer* data, float velocity, float tuneSemitones, float level, float pan = 0.0f);
    void forceStop() noexcept;
    [[nodiscard]] bool isActive() const noexcept { return active_; }

    float renderNextSample() noexcept;
    [[nodiscard]] float lastPanLeft() const noexcept { return panL_; }
    [[nodiscard]] float lastPanRight() const noexcept { return panR_; }

private:
    const TwelveBitBuffer* data_ = nullptr;
    double phase_ = 0.0;
    double phaseInc_ = 1.0;
    float level_ = 1.0f;
    float panL_ = 1.0f;
    float panR_ = 1.0f;
    bool active_ = false;
};

} // namespace sp1200
