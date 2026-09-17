#include "FMVoice.h"

namespace characterfm::dsp
{

void FMVoice::reset (double sampleRate) noexcept
{
    sampleRate_ = sampleRate;
    active_ = false;
    for (auto& op : ops_)
        op.reset (sampleRate);
}

void FMVoice::refreshGraph() noexcept
{
    graph_ = getAlgorithmGraph (params_.algorithmIndex);
}

void FMVoice::setAlgorithmIndex (int index) noexcept
{
    params_.algorithmIndex = index;
    refreshGraph();
}

void FMVoice::setVoiceParams (const VoiceParams& params) noexcept
{
    params_ = params;
    refreshGraph();
    for (int i = 0; i < kNumOperators; ++i)
    {
        ops_[static_cast<std::size_t> (i)].setParams (params_.operators[static_cast<std::size_t> (i)]);
        ops_[static_cast<std::size_t> (i)].setBaseFrequency (params_.pitchHz);
    }
}

void FMVoice::start (float velocity, const VoiceParams& params) noexcept
{
    setVoiceParams (params);
    for (auto& op : ops_)
        op.start (velocity);
    active_ = true;
}

void FMVoice::stop() noexcept
{
    for (auto& op : ops_)
        op.stop();
}

float FMVoice::renderSample() noexcept
{
    modAccumulator_.fill (0.0f);

    if (! active_)
        return 0.0f;

    // DX7-style evaluation: OP6 → OP1, accumulating phase modulation.
    for (int opIndex = kNumOperators - 1; opIndex >= 0; --opIndex)
    {
        auto& op = ops_[static_cast<std::size_t> (opIndex)];
        const float fb = params_.operators[static_cast<std::size_t> (opIndex)].feedback
                         + graph_.feedback[static_cast<std::size_t> (opIndex)] * op.getLastOutput();
        const float phaseMod = modAccumulator_[static_cast<std::size_t> (opIndex)] + fb;
        const float out = op.render (phaseMod);

        for (int e = 0; e < graph_.edgeCount; ++e)
        {
            const auto& edge = graph_.edges[static_cast<std::size_t> (e)];
            if (edge.source == static_cast<std::uint8_t> (opIndex + 1))
            {
                const int dst = edge.destination - 1;
                if (dst >= 0 && dst < kNumOperators)
                    modAccumulator_[static_cast<std::size_t> (dst)] += out * edge.amount;
            }
        }
    }

    float output = 0.0f;
    for (int i = 0; i < kNumOperators; ++i)
    {
        if ((graph_.carrierMask & (1u << i)) != 0)
            output += ops_[static_cast<std::size_t> (i)].getLastOutput();
    }

    active_ = false;
    for (const auto& op : ops_)
        if (op.isActive())
            active_ = true;

    return output;
}

} // namespace characterfm::dsp
