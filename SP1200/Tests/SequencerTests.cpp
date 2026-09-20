#include "../Source/Engine/PatternSequencer.h"
#include "../Source/Import/TransientChop.h"
#include "../Source/SP1200Constants.h"

#include <cmath>
#include <iostream>
#include <vector>

static bool expect (bool c, const char* m)
{
    if (! c)
        std::cerr << "FAIL: " << m << '\n';
    return c;
}

int main()
{
    bool ok = true;
    ok &= expect (sp1200::quantizeToMultiPitch (3.3f) == 3.0f || sp1200::quantizeToMultiPitch (3.3f) == 4.0f,
                  "quantize chromatic cap");

    sp1200::PatternSequencer seq;
    seq.setCurrentPattern (0);
    seq.setPatternBars (4);
    ok &= expect (seq.pattern (0).totalSteps() == 64, "4 bars x 16 steps");

    std::vector<float> impulse (26040, 0.0f);
    for (int i = 1; i < 8; ++i)
        impulse[static_cast<std::size_t> (i * 3000)] = 1.0f;

    const auto slices = sp1200::detectTransientSlices (impulse.data(),
                                                       impulse.size(),
                                                       0,
                                                       static_cast<std::int64_t> (impulse.size()),
                                                       8,
                                                       sp1200::kSampleRateHz,
                                                       0.03);
    ok &= expect (slices.size() >= 2, "transient slices");

    seq.toggleStep (0, 4, 0.9f, 3.0f, true);
    ok &= expect (seq.hasStep (0, 4), "toggle add");
    seq.toggleStep (0, 4, 0.9f, 3.0f, true);
    ok &= expect (! seq.hasStep (0, 4), "toggle remove");

    seq.setSongSlot (0, 0);
    seq.setSongSlot (1, sp1200::kSongSlotEnd);
    seq.setSongLoop (false);
    seq.setPatternBars (1);
    seq.startSong();
    ok &= expect (seq.isPlaying(), "song starts on first slot");
    std::vector<sp1200::ScheduledHit> hits;
    const double sr = 48000.0;
    const int steps = seq.pattern (0).totalSteps();
    const double samplesPerStep = (60.0 / seq.bpm()) / 16.0 * sr;
    const int totalSamples = static_cast<int> (samplesPerStep * static_cast<double> (steps + 2));
    seq.advance (sr, totalSamples, hits);
    ok &= expect (! seq.isPlaying(), "song stops at END slot");

    seq.clearCurrentPattern();
    seq.toggleStep (0, 0, 1.0f, 0.0f, false);
    seq.startPattern();
    std::vector<sp1200::ScheduledHit> clockHits;
    seq.feedMidiClock (6, clockHits);
    ok &= expect (clockHits.size() == 1, "6 MIDI clocks advance one 1/16 step");

    return ok ? 0 : 1;
}
