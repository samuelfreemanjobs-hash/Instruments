#include "../Source/Engine/SamplerEngine.h"
#include "../Source/Memory/TwelveBitBuffer.h"
#include "../Source/Project/ProjectFile.h"
#include "../Source/SP1200Constants.h"

#include <cassert>
#include <cmath>
#include <iostream>

static sp1200::SampleSegment makeToneSegment (const char* name, std::size_t numSamples)
{
    sp1200::SampleSegment seg;
    seg.name = name;
    seg.bank = 0;
    seg.data.resize (numSamples);
    for (std::size_t i = 0; i < numSamples; ++i)
    {
        const float t = static_cast<float> (i) / static_cast<float> (numSamples);
        const float s = std::sin (t * 40.0f * 3.14159f);
        seg.data.setSample (i, static_cast<std::uint16_t> (2048 + static_cast<int> (s * 1000.0f)));
    }
    return seg;
}

int main()
{
    sp1200::SamplerEngine a;
    auto seg = makeToneSegment ("KICK", 8000);
    const auto idx = a.memoryPool().appendSegment (std::move (seg));
    assert (idx.has_value());
    a.assignSegmentToPad (0, static_cast<int> (*idx));
    a.setPadLevel (0, 1.2f);
    a.setFilterCutoffNorm (0.55f);
    a.setFilterResonance (0.42f);
    a.sequencer().setBpm (110.0);
    a.sequencer().addStep (0, 0, 0.8f, 0.0f);

    juce::MemoryBlock block;
    assert (sp1200::ProjectFile::saveToMemoryBlock (a, block));
    assert (block.getSize() > 64);

    sp1200::SamplerEngine b;
    assert (sp1200::ProjectFile::loadFromMemoryBlock (b, block.getData(), block.getSize()));

    assert (b.memoryPool().getSegment (0) != nullptr);
    assert (b.getPad (0).segmentIndex == 0);
    assert (std::abs (b.getPad (0).level - 1.2f) < 0.001f);
    assert (std::abs (b.getFilterCutoffNorm() - 0.55f) < 0.001f);
    assert (std::abs (b.getFilterResonance() - 0.42f) < 0.001f);
    assert (std::abs (b.sequencer().bpm() - 110.0) < 0.001);
    assert (b.sequencer().pattern (0).steps.size() == 1);

    std::cout << "SP1200ProjectFileTests OK\n";
    return 0;
}
