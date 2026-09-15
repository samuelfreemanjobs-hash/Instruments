#include "SequencerTrack.h"

namespace vmpc::model
{
void SequencerTrack::addEvent(const SequencerEvent& event)
{
    events.push_back(event);
}
} // namespace vmpc::model
