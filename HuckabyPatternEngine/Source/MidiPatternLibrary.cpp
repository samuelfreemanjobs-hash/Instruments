#include "MidiPatternLibrary.h"
#include <algorithm>
#include <cmath>

namespace huckaby
{

namespace
{
constexpr int kTicksPerQuarter = 480;

juce::File trackSubfolder(const juce::File& root, int trackIndex)
{
    return root.getChildFile("track_" + juce::String(trackIndex + 1));
}

void writeOneBarPatternMidi(const juce::File& file, uint64_t bits, const std::array<float, kMaxSteps>& velocities, int numSteps)
{
    juce::MidiMessageSequence seq;

    const int steps = juce::jlimit(1, kMaxSteps, numSteps);
    const double ticksPerStep = static_cast<double>(kTicksPerQuarter) / 4.0;
    const int barTicks = kTicksPerQuarter * 4;

    for (int s = 0; s < steps; ++s)
    {
        if (((bits >> s) & 1ull) == 0)
            continue;

        const int vel = juce::jlimit(1, 127, juce::roundToInt(velocities[static_cast<size_t>(s)] * 127.0f));
        const double tOn = static_cast<double>(s) * ticksPerStep;
        const double tOff = juce::jmin(tOn + ticksPerStep * 0.9, static_cast<double>(barTicks) - 1.0);

        seq.addEvent(juce::MidiMessage::noteOn(1, 36, static_cast<uint8_t>(vel)), tOn);
        seq.addEvent(juce::MidiMessage::noteOff(1, 36), tOff);
    }

    seq.updateMatchedPairs();

    juce::MidiFile mf;
    mf.setTicksPerQuarterNote(kTicksPerQuarter);
    mf.addTrack(seq);

    juce::FileOutputStream out(file);
    if (out.openedOk())
        mf.writeTo(out);
}

void createFactoryPatternsForTrack(const juce::File& folder, int trackIndex)
{
    folder.createDirectory();

    const int n = 12;
    for (int p = 0; p < n; ++p)
    {
        uint64_t bits = 0;
        std::array<float, kMaxSteps> vel {};
        vel.fill(0.9f);

        juce::Random rng(static_cast<juce::int32>(0xC0FFEEu ^ static_cast<juce::uint32>(trackIndex * 131 + p * 17)));

        const int density = 3 + (p % 5);
        for (int k = 0; k < density; ++k)
        {
            const int s = rng.nextInt(16);
            bits |= (1ull << s);
            vel[static_cast<size_t>(s)] = 0.55f + rng.nextFloat() * 0.4f;
        }

        if (trackIndex == 0)
        {
            bits |= (1ull << 0);
            bits |= (1ull << 8);
        }
        else if (trackIndex == 1)
        {
            bits |= (1ull << 4);
            bits |= (1ull << 12);
        }

        auto f = folder.getChildFile(juce::String::formatted("pattern_%03d.mid", p + 1));
        writeOneBarPatternMidi(f, bits, vel, 16);
        juce::ignoreUnused(f);
    }
}

bool readSortedMidiFile(const juce::File& file, juce::MidiFile& outMidi, juce::String& err)
{
    juce::FileInputStream in(file);
    if (!in.openedOk())
    {
        err = "Could not open file.";
        return false;
    }

    juce::MidiFile mf;
    if (!mf.readFrom(in, true, nullptr))
    {
        err = "Not a valid MIDI file.";
        return false;
    }

    outMidi = std::move(mf);
    err = {};
    return true;
}
} // namespace

juce::File getDefaultMidiPatternsRoot()
{
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("HuckabyPatternEngine")
        .getChildFile("MidiPatterns");
}

void ensureFactoryMidiPatternsOnDisk()
{
    const auto root = getDefaultMidiPatternsRoot();
    root.createDirectory();

    for (int t = 0; t < kNumTracks; ++t)
    {
        const auto dir = trackSubfolder(root, t);
        if (!dir.isDirectory())
        {
            dir.createDirectory();
            createFactoryPatternsForTrack(dir, t);
            continue;
        }

        auto mids = dir.findChildFiles(juce::File::findFiles, false, "*.mid");
        if (mids.isEmpty())
            mids = dir.findChildFiles(juce::File::findFiles, false, "*.midi");

        if (mids.isEmpty())
            createFactoryPatternsForTrack(dir, t);
    }
}

bool exportPatternToMidiFile(const juce::File& file,
                             const uint64_t bits,
                             const std::array<float, kMaxSteps>& velocities,
                             const int numSteps,
                             juce::String& outError)
{
    juce::ignoreUnused(outError);
    writeOneBarPatternMidi(file, bits, velocities, numSteps);
    return file.existsAsFile();
}

bool importPatternFromMidiFile(const juce::File& file,
                               uint64_t& outBits,
                               std::array<float, kMaxSteps>& outVelocities,
                               juce::String& outError)
{
    juce::MidiFile midi;
    if (!readSortedMidiFile(file, midi, outError))
        return false;

    outBits = 0;
    outVelocities.fill(0.85f);

    const short tf = midi.getTimeFormat();
    if (tf <= 0)
    {
        outError = "SMPTE timecode MIDI files are not supported; re-export with PPQ ticks.";
        return false;
    }

    const double tpq = static_cast<double>(tf);
    const double stepTicks = tpq / 4.0;
    const double barTicks = tpq * 4.0;

    bool any = false;

    for (int ti = 0; ti < midi.getNumTracks(); ++ti)
    {
        const auto* seq = midi.getTrack(ti);
        if (seq == nullptr)
            continue;

        for (int i = 0; i < seq->getNumEvents(); ++i)
        {
            auto msg = seq->getEventPointer(i)->message;
            if (!msg.isNoteOn())
                continue;

            const double tick = seq->getEventPointer(i)->message.getTimeStamp();
            const double pos = std::fmod(juce::jmax(0.0, tick), barTicks);
            int step = static_cast<int>(std::llround(pos / stepTicks));
            step = juce::jlimit(0, kMaxSteps - 1, step);

            outBits |= (1ull << step);
            const float v = static_cast<float>(msg.getVelocity()) / 127.0f;
            outVelocities[static_cast<size_t>(step)] = juce::jmax(outVelocities[static_cast<size_t>(step)], juce::jlimit(0.05f, 1.0f, v));
            any = true;
        }
    }

    if (!any)
    {
        outError = "No note-on events found in the first bar.";
        return false;
    }

    outError = {};
    return true;
}

bool applyMidiPatternFromFolder(const juce::File& folder,
                                int patternIndex,
                                uint64_t& outBits,
                                std::array<float, kMaxSteps>& outVelocities,
                                juce::String& outError)
{
    if (!folder.isDirectory())
    {
        outError = "Not a folder.";
        return false;
    }

    auto files = folder.findChildFiles(juce::File::findFiles, false, "*.mid");
    if (files.isEmpty())
        files = folder.findChildFiles(juce::File::findFiles, false, "*.midi");

    if (files.isEmpty())
    {
        outError = "No MIDI files in folder.";
        return false;
    }

    std::sort(files.begin(), files.end(), [](const juce::File& a, const juce::File& b) {
        return a.getFileName().compareIgnoreCase(b.getFileName()) < 0;
    });

    const int idx = patternIndex % files.size();
    return importPatternFromMidiFile(files[idx], outBits, outVelocities, outError);
}

} // namespace huckaby
