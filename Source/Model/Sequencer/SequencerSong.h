#pragma once

#include "SequencerTrack.h"
#include <vector>

namespace vmpc::model
{
/** Multi-track song container; playback merges events by tick (Phase 3). */
class SequencerSong
{
public:
    static constexpr int kMaxTracks = 64;

    SequencerTrack& getTrack(int index);
    const SequencerTrack& getTrack(int index) const;

    int getTrackCount() const noexcept { return static_cast<int>(tracks.size()); }
    int addTrack(const juce::String& name);

    int64 getLengthTicks() const noexcept { return lengthTicks; }
    void setLengthTicks(int64 ticks) noexcept { lengthTicks = ticks; }

private:
    std::vector<SequencerTrack> tracks;
    int64 lengthTicks = 0;
};
} // namespace vmpc::model
