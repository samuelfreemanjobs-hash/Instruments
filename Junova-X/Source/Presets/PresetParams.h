#pragma once

#include <string_view>
#include <vector>

namespace junovax::presets
{
struct PresetParams
{
    float masterGain = 0.0f;

    float ampAttack = 0.01f;
    float ampDecay = 0.2f;
    float ampSustain = 0.75f;
    float ampRelease = 0.4f;

    float filtAttack = 0.005f;
    float filtDecay = 0.35f;
    float filtSustain = 0.4f;
    float filtRelease = 0.5f;

    float filterCutoff = 0.65f;
    float filterRes = 0.15f;
    bool hpfEnabled = false;
    float hpfCutoff = 0.2f;

    int chorusMode = 0;
    int voiceMode = 0;

    float lfoRate = 4.2f;
    float lfoDelay = 0.15f;
    float glide = 12.0f;

    float dcoLfoMod = 30.0f;
    float dcoPwm = 65.0f;
    float dcoSubLvl = 80.0f;
    float dcoNoise = 10.0f;

    float vcfEnv = 55.0f;
    float vcfLfo = 33.0f;
    float vcfKey = 85.0f;

    float drift = 14.0f;
    float detune = 6.0f;
    float width = 100.0f;

    float arpRange = 2.0f;
    float arpRate = 0.25f;
};

struct FactoryPreset
{
    std::string_view name;
    std::string_view category;
    PresetParams params;
};

const std::vector<FactoryPreset>& getFactoryPresets() noexcept;

} // namespace junovax::presets
