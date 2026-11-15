/** Offline measurements: aliasing FFT, filter sweep, CPU bench, demo WAVs. */
#include "DSP/WavetableBank.h"
#include "DSP/WavetableBankLegacy.h"
#include "Engine/VoiceManager.h"
#include "Engine/SynthParams.h"
#include "Presets/FactoryPresets.h"

#include <chrono>
#include <cmath>
#include <complex>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace
{

constexpr double kPi = 3.141592653589793;

int midiToNote (int n) { return n; }

float midiToHz (int n)
{
    return static_cast<float> (440.0 * std::pow (2.0, (n - 69) / 12.0));
}

void writeWavMono16 (const std::string& path, const std::vector<float>& samples, int sampleRate)
{
    std::ofstream out (path, std::ios::binary);
    if (! out)
        return;
    const std::uint32_t numSamples = static_cast<std::uint32_t> (samples.size());
    const std::uint16_t channels = 1;
    const std::uint16_t bps = 16;
    const std::uint32_t byteRate = static_cast<std::uint32_t> (sampleRate) * channels * bps / 8;
    const std::uint16_t blockAlign = channels * bps / 8;
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
    out.write (reinterpret_cast<const char*> (&channels), 2);
    out.write (reinterpret_cast<const char*> (&sampleRate), 4);
    out.write (reinterpret_cast<const char*> (&byteRate), 4);
    out.write (reinterpret_cast<const char*> (&blockAlign), 2);
    out.write (reinterpret_cast<const char*> (&bps), 2);
    out.write ("data", 4);
    out.write (reinterpret_cast<const char*> (&dataSize), 4);
    for (float s : samples)
    {
        const float c = std::clamp (s, -1.0f, 1.0f);
        const auto i16 = static_cast<std::int16_t> (std::lround (c * 32767.0f));
        out.write (reinterpret_cast<const char*> (&i16), 2);
    }
}

std::vector<float> renderMorph (wave9090::dsp::WavetableBank& bank, bool legacy, double sr, int note, int n)
{
    std::vector<float> mono (static_cast<std::size_t> (n), 0.0f);
    const double phaseInc = midiToHz (note) / sr;
    double phase = 0.1;
    for (int i = 0; i < n; ++i)
    {
        phase += phaseInc;
        mono[static_cast<std::size_t> (i)] = legacy ? wave9090::dsp::legacySampleMorph (bank, phase, 0.62f, phaseInc)
                                                    : bank.sampleMorph (phase, 0.62f, phaseInc);
    }
    return mono;
}

void writeFftCsv (const std::string& path, const std::vector<float>& x, double sr)
{
    const std::size_t n = x.size();
    std::vector<std::complex<double>> X (n);
    for (std::size_t k = 0; k < n; ++k)
    {
        std::complex<double> sum { 0.0, 0.0 };
        for (std::size_t t = 0; t < n; ++t)
            sum += std::polar (static_cast<double> (x[t]), -2.0 * kPi * static_cast<double> (k * t) / static_cast<double> (n));
        X[k] = sum;
    }
    std::ofstream out (path);
    for (std::size_t k = 0; k < n / 2; ++k)
    {
        const double db = 20.0 * std::log10 (std::abs (X[k]) / static_cast<double> (n) + 1.0e-12);
        const double hz = static_cast<double> (k) * sr / static_cast<double> (n);
        out << hz << ',' << db << '\n';
    }
}

std::vector<float> renderFilterSweep (double sr, int n)
{
    wave9090::engine::VoiceManager voices;
    voices.prepare (sr);
    wave9090::SynthParams p;
    p.filterMode = 1;
    p.ampAttack = 0.01f;
    p.ampDecay = 0.6f;
    p.ampSustain = 0.4f;
    p.ampRelease = 0.2f;
    voices.setParams (p);
    voices.noteOn (69, 0.9f);
    std::vector<float> l (static_cast<std::size_t> (n), 0.0f);
    std::vector<float> r (static_cast<std::size_t> (n), 0.0f);
    for (int i = 0; i < n; ++i)
    {
        p.filterCutoff = static_cast<float> (i) / static_cast<float> (std::max (1, n - 1));
        voices.setParams (p);
        std::vector<float> bl (1, 0.0f), br (1, 0.0f);
        voices.render (bl.data(), br.data(), 1);
        l[static_cast<std::size_t> (i)] = 0.5f * (bl[0] + br[0]);
    }
    return l;
}

} // namespace

int main()
{
    constexpr double sr = 44100.0;
    constexpr int fftSamples = 65536;
    const std::string outDir = "/opt/cursor/artifacts";

    wave9090::dsp::WavetableBank bank;
    bank.rebuild();

    const auto before = renderMorph (bank, true, sr, 96, fftSamples);
    const auto after = renderMorph (bank, false, sr, 96, fftSamples);
    writeFftCsv (outDir + "/wave9090-aliasing-before.csv", before, sr);
    writeFftCsv (outDir + "/wave9090-aliasing-after.csv", after, sr);

    const auto sweep = renderFilterSweep (sr, static_cast<int> (sr * 2.0));
    writeWavMono16 (outDir + "/wave9090-filter-sweep.wav", sweep, static_cast<int> (sr));

    wave9090::engine::VoiceManager bench;
    bench.prepare (sr);
    wave9090::SynthParams p;
    bench.setParams (p);
    for (int n = 36; n < 52; ++n)
        bench.noteOn (n, 0.85f);

    constexpr int benchSamples = 48000 * 3;
    std::vector<float> l (static_cast<std::size_t> (benchSamples), 0.0f);
    std::vector<float> r (static_cast<std::size_t> (benchSamples), 0.0f);
    const auto t0 = std::chrono::steady_clock::now();
    bench.render (l.data(), r.data(), benchSamples);
    const auto t1 = std::chrono::steady_clock::now();
    const double ms = std::chrono::duration<double, std::milli> (t1 - t0).count();
  const double usPerSample = ms * 1000.0 / static_cast<double> (benchSamples);

    std::ofstream benchOut (outDir + "/wave9090-cpu-bench.txt");
    benchOut << "voices=16 samples=" << benchSamples << " ms=" << ms << " us_per_sample=" << usPerSample << '\n';

    const int demos[] = { 0, 6, 11, 16, 22, 28, 31, 32 };
    const int notes[] = { 36, 42, 60, 64, 57, 55, 48, 72 };
    const auto& presets = wave9090::presets::getFactoryPresets();
    for (std::size_t d = 0; d < std::size (demos); ++d)
    {
        const int idx = demos[d];
        if (idx >= static_cast<int> (presets.size()))
            continue;
        wave9090::engine::VoiceManager vm;
        vm.prepare (sr);
        vm.setParams (presets[static_cast<std::size_t> (idx)].params);
        vm.noteOn (notes[d], 0.9f);
        const int n = static_cast<int> (sr * 0.9);
        std::vector<float> ol (static_cast<std::size_t> (n), 0.0f);
        std::vector<float> or_ (static_cast<std::size_t> (n), 0.0f);
        vm.render (ol.data(), or_.data(), n);
        std::vector<float> mono (ol.size());
        const float g = std::pow (10.0f, presets[static_cast<std::size_t> (idx)].params.outputGainDb / 20.0f);
        constexpr float kCeil = 0.9441f;
        for (std::size_t i = 0; i < mono.size(); ++i)
        {
            float s = 0.5f * (ol[i] + or_[i]) * g;
            mono[i] = kCeil * std::tanh (s / kCeil);
        }
        writeWavMono16 (outDir + "/wave9090-elite-demo-" + std::to_string (d + 1) + ".wav", mono, static_cast<int> (sr));
    }

    std::cout << "EliteMeasurements OK us/sample=" << usPerSample << '\n';
    return EXIT_SUCCESS;
}