#pragma once

namespace junovax::dsp
{
/** Playhead snapshot for arpeggiator PPQ sync (one audio block). */
struct ArpHostContext
{
    double bpm = 120.0;
    double ppqPosition = 0.0;
    bool ppqValid = false;
};
} // namespace junovax::dsp
