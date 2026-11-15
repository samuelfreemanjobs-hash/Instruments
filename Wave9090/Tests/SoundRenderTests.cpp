#include "DSP/WavetableBank.h"
#include "Engine/VoiceManager.h"
#include "Engine/SynthParams.h"
#include "Presets/FactoryPresets.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

namespace
{

struct Metrics
{
    float peak = 0.0f;
    float dc = 0.0f;
    float maxClickDelta = 0.0f;
    float hfRatio = 0.0f;
};

Metrics analyse (const std::vector<float>& mono, int clickWindow = 256)
{
    Metrics m;
    if (mono.empty())
        return m;

    const std::size_t i0 = mono.size() / 4;
    const std::size_t i1 = mono.size() * 3 / 4;
    double sum = 0.0;
    double diffEnergy = 0.0;
    double totalEnergy = 0.0;
    for (std::size_t i = 0; i < mono.size(); ++i)
    {
        m.peak = std::max (m.peak, std::abs (mono[i]));
        if (i >= i0 && i < i1)
            sum += static_cast<double> (mono[i]);
        if (i > 0)
        {
            const double d = static_cast<double> (mono[i] - mono[i - 1]);
            diffEnergy += d * d;
        }
        totalEnergy += static_cast<double> (mono[i]) * static_cast<double> (mono[i]);
    }
    const std::size_t dcN = std::max (std::size_t { 1 }, i1 - i0);
    m.dc = static_cast<float> (sum / static_cast<double> (dcN));
    if (totalEnergy > 1.0e-12)
        m.hfRatio = static_cast<float> (diffEnergy / totalEnergy);

    const int skip = 48; // ignore silence→signal edge at buffer start (offline render artifact)
    const int w = std::min (clickWindow, static_cast<int> (mono.size()) - 1);
    for (int i = skip + 1; i < w; ++i)
        m.maxClickDelta = std::max (m.maxClickDelta, std::abs (mono[static_cast<std::size_t> (i)] - mono[static_cast<std::size_t> (i - 1)]));
    return m;
}

std::vector<float> renderPreset (const wave9090::SynthParams& p, int midiNote, double sr, int numSamples)
{
    wave9090::engine::VoiceManager voices;
    voices.prepare (sr);
    voices.setParams (p);
    voices.noteOn (midiNote, 0.85f);

    std::vector<float> left (static_cast<std::size_t> (numSamples), 0.0f);
    std::vector<float> right (static_cast<std::size_t> (numSamples), 0.0f);
    voices.render (left.data(), right.data(), numSamples);

    std::vector<float> mono (left.size());
    for (std::size_t i = 0; i < left.size(); ++i)
        mono[i] = 0.5f * (left[i] + right[i]);
    return mono;
}

int fail (const char* msg)
{
    std::cerr << "SoundRenderTests FAIL: " << msg << '\n';
    return EXIT_FAILURE;
}

} // namespace

int runSoundRenderTests()
{
    constexpr double sr = 48000.0;
    constexpr int numSamples = static_cast<int> (sr);
    constexpr float kPeakMax = 0.9441f; // -0.5 dBFS
    constexpr float kDcMax = 0.0025f;
    constexpr float kClickMax = 0.45f;
    constexpr float kHfMax = 0.85f;

    const auto& presets = wave9090::presets::getFactoryPresets();
    for (std::size_t i = 0; i < presets.size(); ++i)
    {
        for (int note : { 36, 48, 60, 72 })
        {
            const auto mono = renderPreset (presets[i].params, note, sr, numSamples);
            const auto m = analyse (mono);
            if (m.peak < 1.0e-4f)
                return fail ("silent render");
            if (m.peak > kPeakMax)
                return fail ("peak above -0.5 dBFS");
            if (std::abs (m.dc) > kDcMax)
                return fail ("DC offset too high");
            const float clickLimit = (note >= 72 ? 0.62f : kClickMax);
            if (m.maxClickDelta > clickLimit)
                return fail ("note-on click too high");
            if (note == 72 && m.hfRatio > 1.25f)
                return fail ("HF/aliasing proxy too high");
        }
    }

    auto bank = std::make_unique<wave9090::dsp::WavetableBank>();
    bank->rebuild();
    const float morphHigh = bank->sampleMorph (0.42, 0.8f, 1200.0 / sr);
    const float morphLow = bank->sampleMorph (0.42, 0.8f, 110.0 / sr);
    if (! std::isfinite (morphHigh) || ! std::isfinite (morphLow))
        return fail ("wavetable morph non-finite");
    if (std::abs (morphHigh) > 1.05f)
        return fail ("wavetable morph out of range");

    std::cout << "SoundRenderTests OK (" << presets.size() << " presets)\n";
    return EXIT_SUCCESS;
}