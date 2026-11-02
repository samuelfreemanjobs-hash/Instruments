#include "Tone.h"

#include "Assets/RomBank.h"

namespace jdupgraded::dsp
{

void Tone::resolveWaveAndPitch (std::uint8_t midiNote) noexcept
{
    const PcmWaveform* wave = patch_.waveform;
    waveRootMidi_ = 60.0f;

    if (patch_.romBank != nullptr)
    {
        if (patch_.multisampleSetId > 0)
        {
            const auto selection = patch_.romBank->selectForNote (patch_.multisampleSetId, midiNote);
            wave = selection.wave;
            waveRootMidi_ = selection.rootMidiNote;
        }
        else
        {
            const auto selection = patch_.romBank->selectFixedWave (patch_.waveIndex);
            wave = selection.wave;
            waveRootMidi_ = selection.rootMidiNote;
        }
    }

    if (wave != nullptr)
        engine_.setWaveform (*wave);
}

} // namespace jdupgraded::dsp
