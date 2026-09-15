#pragma once

#include <JuceHeader.h>
#include <array>

namespace resonance::model
{
struct StepState
{
    bool active = false;
    uint8_t velocity = 100;
    uint8_t flamOffsetTicks = 0; // future: flam timing in PPQN
};

/** 16-step pad pattern (Model only — no drawing). */
class SixteenStepPattern
{
public:
    static constexpr int kNumSteps = 16;

    StepState& getStep(int index) noexcept { return steps[static_cast<size_t>(index & 15)]; }
    const StepState& getStep(int index) const noexcept { return steps[static_cast<size_t>(index & 15)]; }

    int getCurrentStep() const noexcept { return currentStep; }
    void setCurrentStep(int step) noexcept { currentStep = step & 15; }

    void clear() noexcept
    {
        for (auto& s : steps)
            s = {};
        currentStep = 0;
    }

private:
    std::array<StepState, kNumSteps> steps {};
    int currentStep = 0;
};
} // namespace resonance::model
