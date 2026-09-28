#include "MidiMapping.h"

#include <algorithm>

namespace sp1200
{
MidiMapping::MidiMapping()
{
    resetToDefaults();
}

void MidiMapping::resetToDefaults()
{
    for (int i = 0; i < kNumPads; ++i)
    {
        padNotes_[static_cast<std::size_t> (i)] = kPadNoteStart + i;
        faderCc_[static_cast<std::size_t> (i)] = kFaderCcStart + i;
    }
}

int MidiMapping::padNote (int padIndex) const
{
    if (padIndex < 0 || padIndex >= kNumPads)
        return kPadNoteStart;
    return padNotes_[static_cast<std::size_t> (padIndex)];
}

int MidiMapping::faderCc (int padIndex) const
{
    if (padIndex < 0 || padIndex >= kNumPads)
        return kFaderCcStart;
    return faderCc_[static_cast<std::size_t> (padIndex)];
}

void MidiMapping::setPadNote (int padIndex, int note)
{
    if (padIndex >= 0 && padIndex < kNumPads)
        padNotes_[static_cast<std::size_t> (padIndex)] = std::clamp (note, 0, 127);
}

void MidiMapping::setFaderCc (int padIndex, int cc)
{
    if (padIndex >= 0 && padIndex < kNumPads)
        faderCc_[static_cast<std::size_t> (padIndex)] = std::clamp (cc, 0, 127);
}

void MidiMapping::beginLearnPad (int padIndex)
{
    learnTarget_ = MidiLearnTarget::pad;
    learnPadIndex_ = std::clamp (padIndex, 0, kNumPads - 1);
}

void MidiMapping::beginLearnFader (int padIndex)
{
    learnTarget_ = MidiLearnTarget::fader;
    learnPadIndex_ = std::clamp (padIndex, 0, kNumPads - 1);
}

void MidiMapping::cancelLearn()
{
    learnTarget_ = MidiLearnTarget::none;
}

bool MidiMapping::tryApplyLearn (int noteOrCc, bool isNote)
{
    if (learnTarget_ == MidiLearnTarget::none)
        return false;

    if (learnTarget_ == MidiLearnTarget::pad && isNote)
        setPadNote (learnPadIndex_, noteOrCc);
    else if (learnTarget_ == MidiLearnTarget::fader && ! isNote)
        setFaderCc (learnPadIndex_, noteOrCc);
    else
        return true;

    learnTarget_ = MidiLearnTarget::none;
    return true;
}

} // namespace sp1200
