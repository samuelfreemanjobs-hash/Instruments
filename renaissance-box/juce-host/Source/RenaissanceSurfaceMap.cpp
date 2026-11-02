#include "RenaissanceSurfaceMap.h"

#include <algorithm>

namespace renaissance
{
RenaissanceSurfaceMap::RenaissanceSurfaceMap()
{
    for (int i = 0; i < kNumPads; ++i)
    {
        padNotes_[static_cast<std::size_t> (i)] = kPadNoteStart + i;
        qlinkCc_[static_cast<std::size_t> (i)] = kQlinkCcStart + i;
    }
}

int RenaissanceSurfaceMap::padNote (int padIndex) const noexcept
{
    if (padIndex < 0 || padIndex >= kNumPads)
        return kPadNoteStart;
    return padNotes_[static_cast<std::size_t> (padIndex)];
}

int RenaissanceSurfaceMap::qlinkCc (int padIndex) const noexcept
{
    if (padIndex < 0 || padIndex >= kNumPads)
        return kQlinkCcStart;
    return qlinkCc_[static_cast<std::size_t> (padIndex)];
}

int RenaissanceSurfaceMap::padIndexForNote (int note) const noexcept
{
    for (int i = 0; i < kNumPads; ++i)
        if (padNotes_[static_cast<std::size_t> (i)] == note)
            return i;
    return -1;
}

void RenaissanceSurfaceMap::setMidiChannel (int channel1to16) noexcept
{
    midiChannel_ = std::clamp (channel1to16, 1, 16);
}

bool RenaissanceSurfaceMap::acceptsChannel (int midiChannel1to16) const noexcept
{
    return midiOmni_ || midiChannel1to16 == midiChannel_;
}

} // namespace renaissance
