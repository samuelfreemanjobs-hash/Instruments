#include "SequencerCore.h"

namespace resonance::model
{
SequencerCore::SequencerCore()
{
    reset();
}

void SequencerCore::setBpm(double newBpm) noexcept
{
    bpm = juce::jlimit(20.0, 999.0, newBpm);
    clock.setBpm(bpm);
}

void SequencerCore::prepare(double sampleRate)
{
    clock.setSampleRate(sampleRate);
    clock.setBpm(bpm);
    ticksPerStep = ppqn / 4; // 16th notes
}

void SequencerCore::reset()
{
    clock.reset();
    stepIndex = 0;
    playingStepForUi.store(0);
}

void SequencerCore::processBlock(int numSamples, juce::MidiBuffer& midiOut)
{
    const int ticks = clock.advance(numSamples, ppqn);
    if (ticks <= 0)
        return;

    for (int t = 0; t < ticks; ++t)
    {
        const int64 pos = clock.getTickPosition();
        if (ticksPerStep > 0 && (pos % ticksPerStep) == 0)
        {
            const auto& step = pattern.getStep(stepIndex);
            if (step.active)
            {
                midiOut.addEvent(juce::MidiMessage::noteOn(1, 36, static_cast<juce::uint8>(step.velocity)), 0);
                midiOut.addEvent(juce::MidiMessage::noteOff(1, 36), juce::jmin(numSamples - 1, 64));
            }

            playingStepForUi.store(stepIndex);
            stepIndex = (stepIndex + 1) & 15;
        }
    }
}
} // namespace resonance::model
