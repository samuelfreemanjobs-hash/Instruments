#pragma once

#include "../SP1200Constants.h"

#include <array>
#include <optional>

namespace sp1200
{
enum class MidiLearnTarget
{
    none,
    pad,
    fader
};

enum class MidiClockMode
{
    internal,
    slave,
    master
};

/** Per-pad note / CC overrides (SQ-1 defaults in ctor). */
class MidiMapping
{
public:
    MidiMapping();

    [[nodiscard]] int padNote (int padIndex) const;
    [[nodiscard]] int faderCc (int padIndex) const;

    void setPadNote (int padIndex, int note);
    void setFaderCc (int padIndex, int cc);

    void resetToDefaults();

    void beginLearnPad (int padIndex);
    void beginLearnFader (int padIndex);
    void cancelLearn();
    [[nodiscard]] bool isLearning() const noexcept { return learnTarget_ != MidiLearnTarget::none; }
    [[nodiscard]] MidiLearnTarget learnTarget() const noexcept { return learnTarget_; }
    [[nodiscard]] int learnPadIndex() const noexcept { return learnPadIndex_; }

    /** Returns true if message was consumed by learn. */
    bool tryApplyLearn (int noteOrCc, bool isNote);

    void setClockMode (MidiClockMode mode) { clockMode_ = mode; }
    [[nodiscard]] MidiClockMode clockMode() const noexcept { return clockMode_; }

private:
    std::array<int, kNumPads> padNotes_ {};
    std::array<int, kNumPads> faderCc_ {};
    MidiLearnTarget learnTarget_ = MidiLearnTarget::none;
    int learnPadIndex_ = 0;
    MidiClockMode clockMode_ = MidiClockMode::internal;
};

inline constexpr int kMidiClocksPerSixteenth = 6; // 24 PPQN, 4 sixteenths per quarter

} // namespace sp1200
