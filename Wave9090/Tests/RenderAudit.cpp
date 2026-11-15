/** Offline render + analysis for WAVE-9090 factory presets (sound audit / regression). */
#include "DSP/WavetableBank.h"
#include "Engine/VoiceManager.h"
#include "Engine/SynthParams.h"
#include "Presets/FactoryPresets.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace
{

struct Metrics
{
    float peak = 0.0f;
    float peakDb = -120.0f;
    float rms = 0.0f;
    float rmsDb = -120.0f;
    float dc = 0.0f;
    float maxClickDelta = 0.0f;
    float hfRatio = 0.0f;
};

float toDb (float linear)
{
    if (linear <= 1.0e-12f)
        return -120.0f;
    return 20.0f * std::log10 (linear);
}

Metrics analyse (const std::vector<float>& mono, double sampleRate, int clickWindow = 256)
{
    Metrics m;
    if (mono.empty())
        return m;

    double sum = 0.0;
    double sumSq = 0.0;
    for (float s : mono)
    {
        m.peak = std::max (m.peak, std::abs (s));
        sum += static_cast<double> (s);
        sumSq += static_cast<double> (s) * static_cast<double> (s);
    }
    m.peakDb = toDb (m.peak);
    m.rms = static_cast<float> (std::sqrt (sumSq / static_cast<double> (mono.size())));
    m.rmsDb = toDb (m.rms);
    m.dc = static_cast<float> (sum / static_cast<double> (mono.size()));

    const int w = std::min (clickWindow, static_cast<int> (mono.size()) - 1);
    for (int i = 1; i < w; ++i)
        m.maxClickDelta = std::max (m.maxClickDelta, std::abs (mono[static_cast<std::size_t> (i)] - mono[static_cast<std::size_t> (i - 1)]));

    // Crude HF aliasing proxy: |sample[n] - sample[n-1]| energy in upper third of spectrum via diff operator
    double diffEnergy = 0.0;
    double totalEnergy = 0.0;
    for (std::size_t i = 1; i < mono.size(); ++i)
    {
        const double d = static_cast<double> (mono[i] - mono[i - 1]);
        diffEnergy += d * d;
        totalEnergy += static_cast<double> (mono[i]) * static_cast<double> (mono[i]);
    }
    if (totalEnergy > 1.0e-12)
        m.hfRatio = static_cast<float> (diffEnergy / totalEnergy);

    (void) sampleRate;
    return m;
}

void writeWavMono16 (const std::string& path, const std::vector<float>& samples, int sampleRate)
{
    std::ofstream out (path, std::ios::binary);
    if (! out)
        return;

    const std::uint32_t numSamples = static_cast<std::uint32_t> (samples.size());
    const std::uint16_t numChannels = 1;
    const std::uint16_t bitsPerSample = 16;
    const std::uint32_t byteRate = sampleRate * numChannels * bitsPerSample / 8;
    const std::uint16_t blockAlign = numChannels * bitsPerSample / 8;
    const std::uint32_t dataSize = numSamples * blockAlign;
    const std::uint32_t chunkSize = 36 + dataSize;

    out.write ("RIFF", 4);
    out.write (reinterpret_cast<const char*> (&chunkSize), 4);
    out.write ("WAVE", 4);
    out.write ("fmt ", 4);
    const std::uint32_t fmtSize = 16;
    const std::uint16_t audioFormat = 1;
    out.write (reinterpret_cast<const char*> (&fmtSize), 4);
    out.write (reinterpret_cast<const char*> (&audioFormat), 2);
    out.write (reinterpret_cast<const char*> (&numChannels), 2);
    out.write (reinterpret_cast<const char*> (&sampleRate), 4);
    out.write (reinterpret_cast<const char*> (&byteRate), 4);
    out.write (reinterpret_cast<const char*> (&blockAlign), 2);
    out.write (reinterpret_cast<const char*> (&bitsPerSample), 2);
    out.write ("data", 4);
    out.write (reinterpret_cast<const char*> (&dataSize), 4);

    for (float s : samples)
    {
        const float clamped = std::clamp (s, -1.0f, 1.0f);
        const auto i16 = static_cast<std::int16_t> (std::lround (clamped * 32767.0f));
        out.write (reinterpret_cast<const char*> (&i16), 2);
    }
}

std::vector<float> renderPreset (const wave9090::SynthParams& p, int midiNote, float velocity, double sr, int numSamples)
{
    wave9090::engine::VoiceManager voices;
    voices.prepare (sr);
    voices.setParams (p);
    voices.noteOn (midiNote, velocity);

    std::vector<float> left (static_cast<std::size_t> (numSamples), 0.0f);
    std::vector<float> right (static_cast<std::size_t> (numSamples), 0.0f);
    voices.render (left.data(), right.data(), numSamples);

    std::vector<float> mono (left.size());
    for (std::size_t i = 0; i < left.size(); ++i)
        mono[i] = 0.5f * (left[i] + right[i]);
    return mono;
}

std::string slug (std::string_view name)
{
    std::string s;
    for (char c : name)
    {
        if (std::isalnum (static_cast<unsigned char> (c)))
            s += static_cast<char> (std::tolower (static_cast<unsigned char> (c)));
        else if (c == ' ' || c == '-')
            s += '-';
    }
    return s;
}

} // namespace

int main (int argc, char** argv)
{
    const char* outDir = (argc > 1) ? argv[1] : "/opt/cursor/artifacts/wave9090-audit";
    const bool writeWavs = (argc <= 2) || (std::string (argv[2]) != "--no-wav");

    constexpr double sr = 48000.0;
    constexpr int numSamples = static_cast<int> (sr * 1.5);
    constexpr int notes[] = { 36, 48, 60, 72 };

    std::cout << "WAVE9090_RENDER_AUDIT\n";
    std::cout << std::fixed << std::setprecision (4);

    const auto& presets = wave9090::presets::getFactoryPresets();
    for (std::size_t pi = 0; pi < presets.size(); ++pi)
    {
        const auto& preset = presets[pi];
        for (int note : notes)
        {
            const auto mono = renderPreset (preset.params, note, 0.9f, sr, numSamples);
            const auto m = analyse (mono, sr);

            if (writeWavs)
            {
                const std::string path = std::string (outDir) + "/preset" + std::to_string (pi) + "_"
                                         + slug (preset.name) + "_n" + std::to_string (note) + ".wav";
                writeWavMono16 (path, mono, static_cast<int> (sr));
            }

            std::cout << "PRESET\t" << pi << "\t" << preset.name << "\tNOTE\t" << note << "\t"
                      << "peak_dBFS\t" << m.peakDb << "\t"
                      << "rms_dBFS\t" << m.rmsDb << "\t"
                      << "dc\t" << m.dc << "\t"
                      << "click_delta\t" << m.maxClickDelta << "\t"
                      << "hf_diff_ratio\t" << m.hfRatio << '\n';
        }
    }

    // Default init params (engine defaults, not a named preset)
    wave9090::SynthParams defaults;
    for (int note : { 36, 60 })
    {
        const auto mono = renderPreset (defaults, note, 1.0f, sr, numSamples);
        const auto m = analyse (mono, sr);
        std::cout << "DEFAULT\tINIT\tNOTE\t" << note << "\t"
                  << "peak_dBFS\t" << m.peakDb << "\t"
                  << "rms_dBFS\t" << m.rmsDb << "\t"
                  << "dc\t" << m.dc << "\t"
                  << "click_delta\t" << m.maxClickDelta << "\t"
                  << "hf_diff_ratio\t" << m.hfRatio << '\n';
    }

    return EXIT_SUCCESS;
}