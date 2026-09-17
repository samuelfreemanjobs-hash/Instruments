#pragma once

#include "AlgorithmData.h"
#include "ModulationGraph.h"
#include "Operator.h"

#include <array>

namespace characterfm::dsp
{

struct VoiceParams
{
    std::array<OperatorParams, kNumOperators> operators{};
    int algorithmIndex = 0;
    float pitchHz = 440.0f;
};

class FMVoice
{
public:
    void reset (double sampleRate) noexcept;
    void start (float velocity, const VoiceParams& params) noexcept;
    void stop() noexcept;
    bool isActive() const noexcept { return active_; }

    void setVoiceParams (const VoiceParams& params) noexcept;
    void setAlgorithmIndex (int index) noexcept;

    float renderSample() noexcept;

private:
    double sampleRate_ = 44100.0;
    bool active_ = false;
    VoiceParams params_{};
    ModulationGraph graph_{};
    std::array<Operator, kNumOperators> ops_{};
    std::array<float, kNumOperators> modAccumulator_{};

    void refreshGraph() noexcept;
};

} // namespace characterfm::dsp
