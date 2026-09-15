#include "ElectribeSequencer.h"
#include "Audio/RealTimeConstraints.h"

namespace resonance::model
{
void ElectribeSequencer::setBpm(double newBpm) noexcept
{
    bpm = juce::jlimit(20.0, 999.0, newBpm);
    clock.setBpm(bpm);
}

void ElectribeSequencer::prepare(double sampleRate)
{
    clock.setSampleRate(sampleRate);
    clock.setBpm(bpm);
    ticksPerStep = ppqn / 4;
}

void ElectribeSequencer::reset()
{
    clock.reset();
    stepIndex = 0;
    playingStepForUi.store(0);
}

void ElectribeSequencer::processBlock(int numSamples, juce::MidiBuffer& midiOut)
{
    if (song == nullptr)
        return;

    const int ticks = clock.advance(numSamples, ppqn);
    if (ticks <= 0)
        return;

    for (int t = 0; t < ticks; ++t)
    {
        const int64 pos = clock.getTickPosition();
        if (ticksPerStep > 0 && (pos % ticksPerStep) == 0)
        {
            playingStepForUi.store(stepIndex);

            for (int p = 0; p < ElectribeSong::kNumParts; ++p)
            {
                const auto& part = song->getPart(p);
                if (part.muted)
                    continue;

                const auto& step = part.steps[static_cast<size_t>(stepIndex)];
                if (!step.active)
                    continue;

                const int vel = step.accent ? juce::jmin(127, static_cast<int>(step.velocity) + 20)
                                            : static_cast<int>(step.velocity);
                midiOut.addEvent(juce::MidiMessage::noteOn(1, part.midiNote, static_cast<juce::uint8>(vel)), 0);
                midiOut.addEvent(juce::MidiMessage::noteOff(1, part.midiNote), juce::jmin(numSamples - 1, 48));
            }

            stepIndex = (stepIndex + 1) & 15;
        }
    }
}
} // namespace resonance::model
