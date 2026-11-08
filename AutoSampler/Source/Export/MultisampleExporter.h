#pragma once

#include "../Engine/MultisampleProgram.h"

#include <JuceHeader.h>

#include <vector>

namespace autosampler
{

struct ExportResult
{
    bool ok = false;
    juce::String error;
    juce::File presetJson;
};

struct BankExportSpec
{
    juce::String bankName { "Bank" };
    std::vector<MultisampleProgram> presets;
};

/** Writes portable preset folders (JSON, SFZ, MPC, EXS) + bank manifests. */
class MultisampleExporter
{
public:
    static constexpr const char* kPresetFormat = "instruments-autosampler-v1";
    static constexpr const char* kBankFormat = "instruments-autosampler-bank-v1";

    static ExportResult exportPreset (const MultisampleProgram& program, const juce::File& outputDirectory);
    static ExportResult exportMpcKit (const MultisampleProgram& program, const juce::File& outputDirectory, bool includeReadme = false);
    static ExportResult exportBank (const BankExportSpec& bank, const juce::File& outputDirectory);

    static juce::var presetToVar (const MultisampleProgram& program);
    static bool writePresetJson (const MultisampleProgram& program, const juce::File& presetJsonFile);
    static bool writePresetSfz (const MultisampleProgram& program, const juce::File& presetSfzFile);

    static bool writeWavZones (const MultisampleProgram& program, const juce::File& samplesDir, juce::String& error);

    static juce::String sampleFileName (int rootNote, int velStart = -1);
};

} // namespace autosampler
