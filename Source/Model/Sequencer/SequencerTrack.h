#pragma once

#include "SequencerCore.h"
#include <vector>

namespace vmpc::model
{
/** One arrangement track with PPQN-timestamped events (Phase 3). */
class SequencerTrack
{
public:
    juce::String name { "Track" };
    int midiChannel = 1;

    void addEvent(const SequencerEvent& event);
    const std::vector<SequencerEvent>& getEvents() const noexcept { return events; }

    void clear() noexcept { events.clear(); }

private:
    std::vector<SequencerEvent> events;
};
} // namespace vmpc::model
