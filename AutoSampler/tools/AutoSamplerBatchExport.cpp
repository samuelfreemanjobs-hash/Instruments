#include "../Source/Export/MultisampleExporter.h"

#include <juce_core/juce_core.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include <cmath>
#include <iostream>

namespace
{

void printUsage()
{
    std::cerr << "Usage: AutoSamplerBatchExport <bank.json> <outputDirectory>\n"
              << "  Each preset entry: { \"name\", \"synthetic\": true, \"lowNote\", \"highNote\", \"noteStep\" }\n";
}

autosampler::MultisampleProgram makeSyntheticPreset (const juce::String& name, int lowNote, int highNote, int step)
{
    autosampler::MultisampleProgram program;
    program.name = name;
    const double sr = 44100.0;
    const int length = static_cast<int> (sr * 0.25);

    for (int note = lowNote; note <= highNote; note += step)
    {
        autosampler::Zone zone;
        zone.rootNote = note;
        zone.sampleRate = sr;
        zone.audio.setSize (1, length);
        const float freq = 440.0f * std::pow (2.0f, (note - 69) / 12.0f);
        for (int i = 0; i < length; ++i)
        {
            const float t = static_cast<float> (i) / static_cast<float> (length);
            const float s = std::sin (juce::MathConstants<float>::twoPi * freq * (static_cast<float> (i) / static_cast<float> (sr)));
            zone.audio.setSample (0, i, s * (1.0f - t));
        }
        program.addZone (std::move (zone));
    }

    return program;
}

} // namespace

int main (int argc, char** argv)
{
    juce::ScopedJuceInitialiser_GUI juceInit;

    if (argc < 3)
    {
        printUsage();
        return 1;
    }

    const juce::File bankFile (argv[1]);
    const juce::File outDir (argv[2]);

    if (! bankFile.existsAsFile())
    {
        std::cerr << "Missing bank file: " << bankFile.getFullPathName() << '\n';
        return 1;
    }

    const juce::var parsed = juce::JSON::parse (bankFile);
    if (parsed.isVoid())
    {
        std::cerr << "Invalid JSON\n";
        return 1;
    }

    autosampler::BankExportSpec bank;
    bank.bankName = parsed.getProperty ("name", "BatchBank").toString();

    if (auto* presets = parsed.getProperty ("presets", {}).getArray())
    {
        for (const auto& entry : *presets)
        {
            const juce::String name = entry.getProperty ("name", "Preset").toString();
            if (entry.getProperty ("synthetic", false))
            {
                const int lo = static_cast<int> (entry.getProperty ("lowNote", 36));
                const int hi = static_cast<int> (entry.getProperty ("highNote", 48));
                const int step = static_cast<int> (entry.getProperty ("noteStep", 1));
                bank.presets.push_back (makeSyntheticPreset (name, lo, hi, step));
            }
            else
            {
                std::cerr << "Skipping non-synthetic preset '" << name
                          << "' (capture presets in-plugin, then export bank from the UI).\n";
            }
        }
    }

    if (bank.presets.empty())
    {
        std::cerr << "No presets to export\n";
        return 1;
    }

    const auto result = autosampler::MultisampleExporter::exportBank (bank, outDir);
    if (! result.ok)
    {
        std::cerr << result.error << '\n';
        return 1;
    }

    std::cout << "Exported bank to " << result.presetJson.getFullPathName() << '\n';
    return 0;
}
