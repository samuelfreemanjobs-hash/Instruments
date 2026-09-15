#pragma once

#include <JuceHeader.h>
#include <array>

namespace vmpc::model
{
struct ElectribeStep
{
    bool active = false;
    uint8_t velocity = 100;
    bool accent = false;
    bool motionLock = false; // Electribe-style motion sequence latch (Phase 5)
};

struct ElectribePart
{
    juce::String name { "PART" };
    int midiNote = 36;
    bool muted = false;
    float level = 0.8f;
    std::array<ElectribeStep, 16> steps {};
};

/**
 * KORG Electribe-style song: multiple parts sharing one 16-step transport.
 * This is the canonical sequencer when AppMode::Electribe is active.
 */
class ElectribeSong
{
public:
    static constexpr int kNumParts = 8;
    static constexpr int kSteps = 16;

    ElectribePart& getPart(int index) noexcept { return parts[static_cast<size_t>(index & 7)]; }
    const ElectribePart& getPart(int index) const noexcept { return parts[static_cast<size_t>(index & 7)]; }

    int getSelectedPart() const noexcept { return selectedPart; }
    void setSelectedPart(int part) noexcept { selectedPart = juce::jlimit(0, kNumParts - 1, part); }

    bool isMotionSeqEnabled() const noexcept { return motionSeqEnabled; }
    void setMotionSeqEnabled(bool on) noexcept { motionSeqEnabled = on; }

private:
    std::array<ElectribePart, kNumParts> parts {};
    int selectedPart = 0;
    bool motionSeqEnabled = false;
};
} // namespace vmpc::model
