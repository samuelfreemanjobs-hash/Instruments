#include "../Source/Engine/AutoRecordEngine.h"
#include "../Source/Engine/MultisampleProgram.h"
#include "../Source/Engine/NoteMapBuilder.h"
#include "../Source/Engine/SamplePostProcessor.h"
#include "../Source/Export/MultisampleExporter.h"
#include "../Source/Export/MpcKeygroupExporter.h"
#include "../Source/Export/MpcKeyzoneValidator.h"

#include <juce_core/juce_core.h>

#include <cassert>
#include <cmath>
#include <iostream>

static void testNoteMap()
{
    const auto notes = autosampler::NoteMapBuilder::buildNoteList (36,
                                                                     60,
                                                                     1,
                                                                     autosampler::NoteMapPreset::minorThird,
                                                                     autosampler::BlackKeyPolicy::skip);
    assert (notes.size() >= 2);
    for (int n : notes)
        assert (! autosampler::NoteMapBuilder::isBlackKey (n));
}

static void testMpcKeyzones()
{
    autosampler::MultisampleProgram program;
    program.name = "PushFixture";
    for (int root : { 48, 69, 90 })
    {
        autosampler::Zone z;
        z.rootNote = root;
        z.sampleRate = 44100.0;
        z.audio.setSize (1, 128);
        program.addZone (std::move (z));
    }

    const auto v = autosampler::MpcKeyzoneValidator::validateProgram (program);
    assert (program.lowKeyForZone (0) == 0);
    assert (program.highKeyForZone (1) == 79);
    assert (program.lowKeyForZone (2) == 80);
    assert (v.ok);
}

static void testProgram()
{
    autosampler::MultisampleProgram program;

    for (int root : { 36, 48, 60 })
    {
        autosampler::Zone z;
        z.rootNote = root;
        z.sampleRate = 44100.0;
        z.audio.setSize (1, 128);
        program.addZone (std::move (z));
    }

    assert (program.zoneIndexForNote (40) == 0);
    assert (program.zoneIndexForNote (50) == 1);
    assert (program.zoneIndexForNote (72) == 2);

    const auto tree = program.toValueTree();
    autosampler::MultisampleProgram loaded;
    assert (loaded.loadFromValueTree (tree));
    assert (loaded.zoneCount() == 3);
}

static void testAutoRecord()
{
    autosampler::AutoRecordEngine engine;
    engine.prepare (44100.0, 512);

    autosampler::AutoRecordEngine::Config cfg;
    cfg.lowNote = 60;
    cfg.highNote = 62;
    cfg.noteStep = 1;
    cfg.noteHoldMs = 25.0;
    cfg.tailMs = 25.0;
    cfg.pauseMs = 10.0;
    cfg.midiThru = true;

    autosampler::MultisampleProgram program;
    engine.start (cfg);

    juce::AudioBuffer<float> input (2, 512);
    juce::MidiBuffer midi;

    for (int block = 0; block < 200 && engine.state() != autosampler::AutoSampleState::finished; ++block)
    {
        input.clear();
        for (int i = 0; i < input.getNumSamples(); ++i)
            input.setSample (0, i, 0.25f);

        engine.process (input, midi, program);
    }

    assert (engine.state() == autosampler::AutoSampleState::finished);
    assert (program.zoneCount() == 3);
}

static void testPostProcessAndSfz()
{
    autosampler::Zone zone;
    zone.rootNote = 60;
    zone.sampleRate = 44100.0;
    const int n = static_cast<int> (zone.sampleRate);
    zone.audio.setSize (1, n);

    for (int i = 0; i < n; ++i)
    {
        float s = 0.0f;
        if (i > 2000 && i < n - 4000)
            s = 0.35f * std::sin (juce::MathConstants<float>::twoPi * 220.0f * (static_cast<float> (i) / static_cast<float> (zone.sampleRate)));
        zone.audio.setSample (0, i, s);
    }

    autosampler::PostProcessConfig cfg;
    cfg.trimSilence = true;
    cfg.normalize = true;
    cfg.findSustainLoop = true;
    cfg.minLoopConfidence = 0.2f;
    autosampler::SamplePostProcessor::processZone (zone, cfg);

    assert (zone.audio.getNumSamples() < n);
    assert (zone.loopStart >= 0 || zone.loopConfidence >= 0.0f);

    autosampler::MultisampleProgram program;
    program.name = "LoopPreset";
    program.addZone (std::move (zone));

    const juce::File out = juce::File::getSpecialLocation (juce::File::tempDirectory).getChildFile ("autosampler_sfz_test");
    out.deleteRecursively();
    const auto result = autosampler::MultisampleExporter::exportPreset (program, out);
    assert (result.ok);
    const juce::File sfz = result.presetJson.getParentDirectory().getChildFile ("LoopPreset.sfz");
    assert (sfz.existsAsFile());
    assert (sfz.loadFileAsString().contains ("loop_start="));

    juce::String mpcErr;
    assert (autosampler::MpcKeygroupExporter::exportKeygroup (program, out.getChildFile ("mpc_only"), mpcErr));
    const juce::File xpm = out.getChildFile ("mpc_only").getChildFile ("LoopPreset").getChildFile ("LoopPreset.xpm");
    assert (xpm.existsAsFile());
    assert (xpm.loadFileAsString().contains ("MPCVObject"));
    assert (xpm.loadFileAsString().contains ("KeygroupMasterTranspose"));
    assert (xpm.loadFileAsString().contains ("<SampleFile>"));

    out.deleteRecursively();
}

static void testExporter()
{
    autosampler::MultisampleProgram program;
    program.name = "TestPreset";

    autosampler::Zone z;
    z.rootNote = 60;
    z.sampleRate = 44100.0;
    z.audio.setSize (2, 256);
    z.audio.clear();
    program.addZone (std::move (z));

    const juce::File out = juce::File::getSpecialLocation (juce::File::tempDirectory)
                               .getChildFile ("autosampler_export_test");
    out.deleteRecursively();

    const auto result = autosampler::MultisampleExporter::exportPreset (program, out);
    assert (result.ok);
    assert (result.presetJson.existsAsFile());

    autosampler::BankExportSpec bank;
    bank.bankName = "TestBank";
    bank.presets.push_back (program);

    const auto bankResult = autosampler::MultisampleExporter::exportBank (bank, out.getChildFile ("bank_root"));
    assert (bankResult.ok);

    out.deleteRecursively();
}

int main()
{
    juce::ScopedJuceInitialiser_GUI juceInit;
    testNoteMap();
    testMpcKeyzones();
    testProgram();
    testAutoRecord();
    testPostProcessAndSfz();
    testExporter();
    std::cout << "AutoSamplerTests OK\n";
    return 0;
}
