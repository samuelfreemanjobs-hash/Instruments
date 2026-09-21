#include "ProjectFile.h"

#include "../Engine/MidiMapping.h"
#include "../Engine/PadAssignments.h"
#include "../Engine/SamplerEngine.h"

namespace sp1200
{
namespace
{
constexpr std::uint32_t kMagic = 0x53503132; // 'SP12'
constexpr std::uint16_t kVersion = 5;

void writeString (juce::MemoryOutputStream& out, const std::string& s)
{
    out.writeInt (static_cast<int> (s.size()));
    out.write (s.data(), s.size());
}

std::string readString (juce::MemoryInputStream& in)
{
    const int len = in.readInt();
    if (len <= 0 || len > 65536)
        return {};
    std::string s (static_cast<std::size_t> (len), '\0');
    in.read (s.data(), len);
    return s;
}
} // namespace

bool ProjectFile::saveToMemoryBlock (const SamplerEngine& engine, juce::MemoryBlock& dest)
{
    juce::MemoryOutputStream out (dest, false);

    out.writeInt (static_cast<int> (kMagic));
    out.writeShort (static_cast<int> (kVersion));

    const auto& pool = engine.memoryPool();
    out.writeInt (static_cast<int> (pool.usedSamples())); // metadata

    // segments — need segment count via iterating - add segmentCount to pool
    const auto& segs = engine.memoryPool();
    // Hack: we don't expose segment count - use getSegment until null
    std::vector<const SampleSegment*> all;
    for (std::size_t i = 0;; ++i)
    {
        const auto* s = segs.getSegment (i);
        if (s == nullptr)
            break;
        all.push_back (s);
    }
    out.writeInt (static_cast<int> (all.size()));
    for (const auto* seg : all)
    {
        writeString (out, seg->name);
        out.writeInt (seg->bank);
        out.writeInt64 (static_cast<juce::int64> (seg->data.size()));
        const auto packed = seg->data.pack();
        out.writeInt (static_cast<int> (packed.size()));
        out.write (packed.data(), packed.size());
    }

    for (int p = 0; p < kNumPads; ++p)
    {
        const auto pad = engine.getPad (p);
        out.writeInt (pad.segmentIndex);
        out.writeFloat (pad.tuneSemitones);
        out.writeFloat (pad.level);
        out.writeFloat (pad.decay);
        out.writeInt (pad.chokeGroup);
    }

    out.writeBool (engine.multiPitchEnabled());
    out.writeInt (engine.multiPitchSourceSegment());

    const auto& seq = engine.sequencer();
    out.writeInt (seq.currentPattern());
    for (int pi = 0; pi < kMaxPatterns; ++pi)
    {
        const auto& pat = seq.pattern (pi);
        out.writeInt (pat.bars);
        out.writeInt (static_cast<int> (pat.steps.size()));
        for (const auto& st : pat.steps)
        {
            out.writeInt (st.pad);
            out.writeInt (st.stepIndex);
            out.writeFloat (st.velocity);
            out.writeFloat (st.tuneSemitones);
            out.writeFloat (st.pan);
            out.writeFloat (st.filterCutoff);
        }
    }

    for (int i = 0; i < 32; ++i)
        out.writeInt (seq.songSlot (i));

    out.writeDouble (seq.bpm());
    out.writeFloat (seq.swing());
    out.writeFloat (engine.getFilterCutoffNorm());
    out.writeFloat (engine.getFilterResonance());
    out.writeBool (seq.songLoop());
    out.writeInt (engine.midiChannel());
    out.writeBool (engine.midiOmni());
    out.writeBool (engine.vinylImportEnabled());
    out.writeInt (static_cast<int> (engine.midiMapping().clockMode()));
    for (int p = 0; p < kNumPads; ++p)
    {
        out.writeInt (engine.midiMapping().padNote (p));
        out.writeInt (engine.midiMapping().faderCc (p));
    }
    out.writeInt (engine.currentBank());
    out.writeInt (engine.selectedPad());

    return true;
}

bool ProjectFile::loadFromMemoryBlock (SamplerEngine& engine, const void* data, std::size_t size)
{
    juce::MemoryInputStream in (data, size, false);
    if (in.readInt() != static_cast<int> (kMagic))
        return false;
    const int fileVersion = in.readShort();
    if (fileVersion < 1 || fileVersion > static_cast<int> (kVersion))
        return false;

    juce::ignoreUnused (in.readInt()); // used samples meta

    const int segCount = in.readInt();
    std::vector<SampleSegment> segments;
    segments.reserve (static_cast<std::size_t> (segCount));
    for (int i = 0; i < segCount; ++i)
    {
        SampleSegment seg;
        seg.name = readString (in);
        seg.bank = in.readInt();
        const auto sampleCount = static_cast<std::size_t> (in.readInt64());
        const int packLen = in.readInt();
        std::vector<std::uint8_t> packed (static_cast<std::size_t> (packLen));
        in.read (packed.data(), packLen);
        seg.data.unpack (packed.data(), packed.size(), sampleCount);
        segments.push_back (std::move (seg));
    }
    engine.memoryPool().replaceAll (std::move (segments));

    for (int p = 0; p < kNumPads; ++p)
    {
        PadAssignment pad;
        pad.segmentIndex = in.readInt();
        pad.tuneSemitones = in.readFloat();
        pad.level = in.readFloat();
        pad.decay = in.readFloat();
        if (fileVersion >= 3)
            pad.chokeGroup = in.readInt();
        engine.setPadAssignment (p, pad);
    }
    if (fileVersion < 3)
        engine.applyDefaultPadChokeGroups();

    engine.setMultiPitchEnabled (in.readBool());
    engine.setMultiPitchSourceSegment (in.readInt());

    auto& seq = engine.sequencer();
    seq.setCurrentPattern (in.readInt());
    for (int pi = 0; pi < kMaxPatterns; ++pi)
    {
        Pattern pat;
        pat.bars = in.readInt();
        const int stepCount = in.readInt();
        pat.steps.reserve (static_cast<std::size_t> (stepCount));
        for (int s = 0; s < stepCount; ++s)
        {
            PatternStep st;
            st.pad = in.readInt();
            st.stepIndex = in.readInt();
            st.velocity = in.readFloat();
            st.tuneSemitones = in.readFloat();
            st.pan = in.readFloat();
            st.filterCutoff = in.readFloat();
            pat.steps.push_back (st);
        }
        seq.pattern (pi) = std::move (pat);
    }

    for (int i = 0; i < 32; ++i)
        seq.setSongSlot (i, in.readInt());

    seq.setBpm (in.readDouble());
    if (fileVersion >= 2)
        seq.setSwing (in.readFloat());
    engine.setFilterCutoffNorm (in.readFloat());
    engine.setFilterResonance (in.readFloat());
    if (fileVersion >= 3)
    {
        seq.setSongLoop (in.readBool());
        engine.setMidiChannel (in.readInt());
        engine.setMidiOmni (in.readBool());
    }
    if (fileVersion >= 4)
    {
        engine.setVinylImportEnabled (in.readBool());
        engine.midiMapping().setClockMode (static_cast<MidiClockMode> (in.readInt()));
        for (int p = 0; p < kNumPads; ++p)
        {
            engine.midiMapping().setPadNote (p, in.readInt());
            engine.midiMapping().setFaderCc (p, in.readInt());
        }
    }
    if (fileVersion >= 5)
    {
        engine.setCurrentBank (in.readInt());
        engine.setSelectedPad (in.readInt());
    }

    return in.getNumBytesRemaining() >= 0;
}

bool ProjectFile::saveToFile (const SamplerEngine& engine, const juce::File& file)
{
    juce::MemoryBlock block;
    if (! saveToMemoryBlock (engine, block))
        return false;
    return file.replaceWithData (block.getData(), block.getSize());
}

bool ProjectFile::loadFromFile (SamplerEngine& engine, const juce::File& file)
{
    juce::MemoryBlock block;
    if (! file.loadFileAsData (block))
        return false;
    return loadFromMemoryBlock (engine, block.getData(), block.getSize());
}

} // namespace sp1200
