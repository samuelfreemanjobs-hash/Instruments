#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace rev2presets {

struct Patch
{
    const char* name;
    float dcoMix;
    int wave1;
    int wave2;
    float pwm;
    float slop;
    int filterMode;
    float cutoff;
    float res;
    float bassComp;
    float glideMs;
    int satOn;
    float crunch;
    float subLevel;
    float ampA, ampD, ampS, ampR;
    float filtA, filtD, filtS, filtR;
};

inline constexpr Patch kPatches[] = {
    { "Phonk Brass", 0.45f, 0, 0, 0.42f, 0.65f, 0, 350.f, 0.45f, 0.8f, 20.f, 1, 0.6f,
      0.4f, 0.01f, 0.35f, 0.7f, 0.25f, 0.02f, 0.55f, 0.75f, 0.2f },
    { "Rev2 Trap Pluck", 0.2f, 2, 0, 0.15f, 0.2f, 0, 180.f, 0.65f, 0.9f, 0.f, 0, 0.0f,
      0.25f, 0.005f, 0.22f, 0.0f, 0.12f, 0.003f, 0.4f, 0.55f, 0.15f },
    { "Glide Pad", 0.55f, 1, 0, 0.55f, 0.8f, 1, 800.f, 0.15f, 0.2f, 150.f, 1, 0.2f,
      0.3f, 1.2f, 0.5f, 0.85f, 0.9f, 0.15f, 0.35f, 0.65f, 0.45f },
};

inline constexpr int kNumPrograms = static_cast<int>(sizeof(kPatches) / sizeof(kPatches[0]));

inline void applyPatch(juce::AudioProcessorValueTreeState& apvts, int index)
{
    const auto& p = kPatches[static_cast<std::size_t>(index)];
    auto set = [&apvts](const char* id, float v) {
        if (auto* r = apvts.getParameter(id))
            r->setValueNotifyingHost(r->convertTo0to1(v));
    };
    set("dco_mix", p.dcoMix);
    set("dco1_wave", static_cast<float>(p.wave1));
    set("dco2_wave", static_cast<float>(p.wave2));
    set("pwm_width", p.pwm);
    set("slop", p.slop);
    set("filter_mode", static_cast<float>(p.filterMode));
    set("filter_cutoff", p.cutoff);
    set("filter_res", p.res);
    set("bass_comp", p.bassComp);
    set("glide_ms", p.glideMs);
    set("saturation_on", static_cast<float>(p.satOn));
    set("crunch", p.crunch);
    set("sub_level", p.subLevel);
    set("amp_a", p.ampA);
    set("amp_d", p.ampD);
    set("amp_s", p.ampS);
    set("amp_r", p.ampR);
    set("filt_a", p.filtA);
    set("filt_d", p.filtD);
    set("filt_s", p.filtS);
    set("filt_r", p.filtR);
}

} // namespace rev2presets
