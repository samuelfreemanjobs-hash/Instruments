#pragma once

namespace junovax::dsp
{
class DiagTone
{
public:
    void prepare (double sampleRate) noexcept;
    void reset() noexcept;
    float processSample() noexcept;

    void setFrequencyHz (float hz) noexcept { frequencyHz_ = hz; }
    void setEnabled (bool on) noexcept { enabled_ = on; }

private:
    double sampleRate_ = 48000.0;
    float phase_ = 0.0f;
    float frequencyHz_ = 440.0f;
    bool enabled_ = false;
};
} // namespace junovax::dsp
