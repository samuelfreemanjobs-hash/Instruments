#include "../Source/Engine/SamplerEngine.h"
#include "../Source/Memory/TwelveBitBuffer.h"
#include "../Source/Project/ProjectFile.h"
#include "../Source/SP1200Constants.h"

#include <juce_audio_formats/juce_audio_formats.h>

#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>

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
    a.sequencer().setSwing (0.61f);
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
    assert (std::abs (b.sequencer().swing() - 0.61f) < 0.001f);
    assert (b.sequencer().pattern (0).steps.size() == 1);

    {
        const juce::File artifactWav ("/opt/cursor/artifacts/sp1200_p4_kick.wav");
        if (artifactWav.existsAsFile())
        {
            sp1200::SamplerEngine probe;
            const auto probeIdx = probe.importFile (artifactWav, 0, "kick");
            assert (probeIdx.has_value());
        }
    }

    // P4 contract: WAV import (resample to 26.040 kHz) + .sp12p disk round-trip
    {
        const int srcSamples = static_cast<int> (44100 / 5);
        juce::AudioBuffer<float> wavBuf (1, srcSamples);
        auto* ch = wavBuf.getWritePointer (0);
        for (int i = 0; i < srcSamples; ++i)
            ch[i] = 0.45f * std::sin (2.0f * 3.14159f * 110.0f * static_cast<float> (i) / 44100.0f);

        const auto tempDir = juce::File::getSpecialLocation (juce::File::tempDirectory);
        juce::File wavPath = tempDir.getChildFile ("sp1200_ctest_import.wav");
        juce::File projPath = tempDir.getChildFile ("sp1200_ctest_roundtrip.sp12p");
        wavPath.deleteFile();
        projPath.deleteFile();

        juce::WavAudioFormat wavFormat;
        std::unique_ptr<juce::FileOutputStream> stream (wavPath.createOutputStream());
        assert (stream != nullptr);
        std::unique_ptr<juce::AudioFormatWriter> writer (
            wavFormat.createWriterFor (stream.get(), 44100.0, 1, 16, {}, 0));
        assert (writer != nullptr);
        stream.release();
        assert (writer->writeFromAudioSampleBuffer (wavBuf, 0, srcSamples));

        sp1200::SamplerEngine imported;
        const auto segIdx = imported.importFile (wavPath, 0, "ctest");
        assert (segIdx.has_value());
        const auto* seg = imported.memoryPool().getSegment (*segIdx);
        assert (seg != nullptr);
        assert (seg->data.getNumSamples() > 100);

        assert (sp1200::ProjectFile::saveToFile (imported, projPath));
        sp1200::SamplerEngine reloaded;
        assert (sp1200::ProjectFile::loadFromFile (reloaded, projPath));
        assert (reloaded.memoryPool().getSegment (*segIdx) != nullptr);
        assert (reloaded.memoryPool().getSegment (*segIdx)->data.getNumSamples() == seg->data.getNumSamples());

        wavPath.deleteFile();
        projPath.deleteFile();
    }

    std::cout << "SP1200ProjectFileTests OK\n";
    return 0;
}
