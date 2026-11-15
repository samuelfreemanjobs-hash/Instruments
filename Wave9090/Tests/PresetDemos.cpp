/** Render short WAV demos for representative WAVE-9090 factory presets. */
#include "Engine/VoiceManager.h"
#include "Engine/SynthParams.h"
#include "Presets/FactoryPresets.h"

#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace
{

void writeWavMono16 (const std::string& path, const std::vector<float>& samples, int sampleRate)
{
    std::ofstream out (path, std::ios::binary);
    if (! out)
        return;

    const std::uint32_t numSamples = static_cast<std::uint32_t> (samples.size());
    const std::uint16_t numChannels = 1;
    const std::uint16_t bitsPerSample = 16;
    const std::uint32_t byteRate = static_cast<std::uint32_t> (sampleRate) * numChannels * bitsPerSample / 8;
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

std::vector<float> renderMono (const wave9090::SynthParams& p, int midiNote, double sr, int numSamples)
{
    wave9090::engine::VoiceManager voices;
    voices.prepare (sr);
    voices.setParams (p);
    voices.noteOn (midiNote, 0.9f);

    std::vector<float> left (static_cast<std::size_t> (numSamples), 0.0f);
    std::vector<float> right (static_cast<std::size_t> (numSamples), 0.0f);
    voices.render (left.data(), right.data(), numSamples);

    const float gain = std::pow (10.0f, p.outputGainDb / 20.0f);
    constexpr float kCeil = 0.9441f;

    std::vector<float> mono (left.size());
    for (std::size_t i = 0; i < left.size(); ++i)
    {
        float s = 0.5f * (left[i] + right[i]);
        s *= gain;
        mono[i] = kCeil * std::tanh (s / kCeil);
    }
    return mono;
}

std::string slug (std::string_view name)
{
    std::string s;
    for (char c : name)
    {
        if (std::isalnum (static_cast<unsigned char> (c)))
            s += static_cast<char> (std::tolower (static_cast<unsigned char> (c)));
        else if (c == ' ')
            s += '-';
    }
    return s;
}

} // namespace

int main()
{
    constexpr double sr = 48000.0;
    constexpr int numSamples = static_cast<int> (sr * 0.85);
    constexpr int demoIndices[] = { 0, 5, 10, 15, 23, 28 };
    constexpr int demoNotes[] = { 36, 42, 60, 64, 57, 55 };

    const char* outDir = "/opt/cursor/artifacts";
    const auto& presets = wave9090::presets::getFactoryPresets();

    for (std::size_t d = 0; d < std::size (demoIndices); ++d)
    {
        const int pi = demoIndices[d];
        if (pi < 0 || pi >= static_cast<int> (presets.size()))
            continue;

        const auto& preset = presets[static_cast<std::size_t> (pi)];
        const int note = demoNotes[d];
        const auto mono = renderMono (preset.params, note, sr, numSamples);
        const std::string path = std::string (outDir) + "/wave9090-demo-" + std::to_string (d + 1) + "-"
                                 + slug (preset.name) + ".wav";
        writeWavMono16 (path, mono, static_cast<int> (sr));
        std::cout << "Wrote " << path << " (" << preset.name << " note " << note << ")\n";
    }

    return EXIT_SUCCESS;
}