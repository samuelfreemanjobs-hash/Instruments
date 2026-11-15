#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

#include <array>
#include <vector>

namespace disklordz::chordengine
{

/** Lightweight poly preview synth (RT-safe, fixed voice count). */
class PreviewSynth
{
public:
    static constexpr int kMaxVoices = 12;

    void prepare(double sampleRate);
    void reset();

    void noteOn(int midiNote, float velocity);
    void noteOff(int midiNote);
    void allNotesOff();

    void render(juce::AudioBuffer<float>& buffer);

private:
    struct Voice
    {
        bool active = false;
        int note = 0;
        float phase = 0.0f;
        float phaseInc = 0.0f;
        float amp = 0.0f;
        float targetAmp = 0.0f;
    };

    double sampleRate_ = 44100.0;
    std::array<Voice, kMaxVoices> voices_{};

    int allocateVoice(int midiNote);
    void releaseVoice(int midiNote);
};

} // namespace disklordz::chordengine