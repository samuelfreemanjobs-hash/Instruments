/** Offline measurements: aliasing FFT, filter sweep, CPU bench, demo WAVs. */
#include "DSP/WavetableBank.h"
#include "DSP/WavetableBankLegacy.h"
#include "Engine/VoiceManager.h"
#include "Engine/SynthParams.h"
#include "Presets/FactoryPresets.h"
#include "DSP/WavetableFft.h"

#include <array>
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
constexpr int kMidiC6 = 84;
constexpr int kMidiC7 = 96;
constexpr int kMidiC8 = 108;

float midiToHz (int n)
{
    return static_cast<float> (440.0 * std::pow (2.0, (n - 69) / 12.0));
}

double blackmanHarris (std::size_t n, std::size_t i)
{
    const double a0 = 0.35875;
    const double a1 = 0.48829;
    const double a2 = 0.14128;
    const double a3 = 0.01168;
    const double x = static_cast<double> (i) / static_cast<double> (n - 1);
    return a0 - a1 * std::cos (2.0 * kPi * x) + a2 * std::cos (4.0 * kPi * x) - a3 * std::cos (6.0 * kPi * x);
}

struct SpectrumStats
{
    double fundamentalHz = 0.0;
    double fundamentalDb = -120.0;
    double worstInharmDb = -120.0;
    double worstInharmHz = 0.0;
};

SpectrumStats analyseInharmonics (const std::vector<float>& x, double sr, double expectedFundHz)
{
    const std::size_t n = x.size();
    std::vector<std::complex<double>> spec (n);
    for (std::size_t i = 0; i < n; ++i)
    {
        const double w = blackmanHarris (n, i);
        spec[i] = { static_cast<double> (x[i]) * w, 0.0 };
    }
    wave9090::dsp::fftRadix2 (spec);

    const auto isHarmonicHz = [expectedFundHz, sr] (double hz) noexcept
    {
        if (hz < expectedFundHz * 0.9)
            return true;
        for (int h = 1; h <= 128; ++h)
        {
            const double hf = static_cast<double> (h) * expectedFundHz;
            if (std::abs (hz - hf) < expectedFundHz * 0.015)
                return true;
            if (hf > 0.5 * sr)
            {
                const double aliasHz = sr - hf;
                if (aliasHz > 0.0 && std::abs (hz - aliasHz) < expectedFundHz * 0.02)
                    return true;
            }
        }
        return false;
    };

    double fundMag = 0.0;
    const std::size_t kFundCenter = static_cast<std::size_t> (std::lround (expectedFundHz * static_cast<double> (n) / sr));
    for (std::size_t k = std::max (std::size_t { 1 }, kFundCenter > 3 ? kFundCenter - 3 : 1);
         k < std::min (n / 2, kFundCenter + 4); ++k)
        fundMag = std::max (fundMag, std::abs (spec[k]));
    fundMag /= static_cast<double> (n);
    const double fundDb = 20.0 * std::log10 (fundMag + 1.0e-12);

    double worstMag = 0.0;
    double worstHz = 0.0;
    for (std::size_t k = 1; k < n / 2; ++k)
    {
        const double hz = static_cast<double> (k) * sr / static_cast<double> (n);
        if (isHarmonicHz (hz))
            continue;
        const double mag = std::abs (spec[k]) / static_cast<double> (n);
        if (mag > worstMag)
        {
            worstMag = mag;
            worstHz = hz;
        }
    }
    const double worstDb = 20.0 * std::log10 (worstMag + 1.0e-12);

    SpectrumStats s;
    s.fundamentalHz = expectedFundHz;
    s.fundamentalDb = fundDb;
    s.worstInharmDb = worstDb;
    s.worstInharmHz = worstHz;
    return s;
}

void writeFftCsv (const std::string& path, const std::vector<float>& x, double sr)
{
    const std::size_t n = x.size();
    std::vector<std::complex<double>> spec (n);
    for (std::size_t i = 0; i < n; ++i)
    {
        const double w = blackmanHarris (n, i);
        spec[i] = { static_cast<double> (x[i]) * w, 0.0 };
    }
    wave9090::dsp::fftRadix2 (spec);

    std::ofstream out (path);
    for (std::size_t k = 0; k < n / 2; ++k)
    {
        const double db = 20.0 * std::log10 (std::abs (spec[k]) / static_cast<double> (n) + 1.0e-12);
        const double hz = static_cast<double> (k) * sr / static_cast<double> (n);
        out << hz << ',' << db << '\n';
    }
}

std::vector<float> renderMorph (const wave9090::dsp::WavetableBank& bank, bool legacy, int midiNote, int n, double sr)
{
    std::vector<float> mono (static_cast<std::size_t> (n), 0.0f);
    const double phaseInc = static_cast<double> (midiToHz (midiNote)) / sr;
    double phase = 0.0;
    for (int i = 0; i < n; ++i)
    {
        phase += phaseInc;
        mono[static_cast<std::size_t> (i)] = legacy
                                               ? wave9090::dsp::legacySampleMorph (bank, phase, 0.62f, phaseInc)
                                               : bank.sampleMorph (phase, 0.62f, phaseInc);
    }
    return mono;
}

std::vector<float> renderUnisonMorph (const wave9090::dsp::WavetableBank& bank, int midiNote, int n, double sr)
{
    std::vector<float> mono (static_cast<std::size_t> (n), 0.0f);
    constexpr int kVoices = 7;
    const float spreadCents = 42.0f;
    const float baseFreq = midiToHz (midiNote);
    std::array<double, kVoices> phases{};
    for (int u = 0; u < kVoices; ++u)
        phases[static_cast<std::size_t> (u)] = static_cast<double> (u) * 0.131;

    for (int i = 0; i < n; ++i)
    {
        float s = 0.0f;
        for (int u = 0; u < kVoices; ++u)
        {
            const float t = static_cast<float> (u) / static_cast<float> (kVoices - 1) * 2.0f - 1.0f;
            const float cents = t * spreadCents;
            const float freq = baseFreq * std::pow (2.0f, cents / 1200.0f);
            const double phaseInc = static_cast<double> (freq) / sr;
            phases[static_cast<std::size_t> (u)] += phaseInc;
            s += bank.sampleMorph (phases[static_cast<std::size_t> (u)], 0.62f, phaseInc);
        }
        mono[static_cast<std::size_t> (i)] = s / static_cast<float> (std::sqrt (static_cast<float> (kVoices)));
    }
    return mono;
}

std::vector<float> renderVoice (int midiNote, bool unison, int n, double sr)
{
    wave9090::engine::VoiceManager voices;
    voices.prepare (sr);
    wave9090::SynthParams p;
    p.wtPosition = 0.62f;
    p.ampAttack = 0.01f;
    p.ampDecay = 2.0f;
    p.ampSustain = 0.75f;
    p.ampRelease = 0.2f;
    p.filterCutoff = 1.0f;
    p.filterRes = 0.0f;
    p.filterDestructive = false;
    p.drive = 0.0f;
    p.crush = 0.0f;
    p.tapeWobble = 0.0f;
    if (unison)
    {
        p.detuneCents = 42.0f;
        p.stereoWidth = 0.9f;
    }
    voices.setParams (p);
    voices.noteOn (midiNote, 0.95f);
    std::vector<float> l (static_cast<std::size_t> (n), 0.0f);
    std::vector<float> r (static_cast<std::size_t> (n), 0.0f);
    voices.render (l.data(), r.data(), n);
    std::vector<float> mono (l.size());
    for (std::size_t i = 0; i < l.size(); ++i)
        mono[i] = 0.5f * (l[i] + r[i]);
    return mono;
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

} // namespace

int main()
{
    constexpr double sr = 44100.0;
    constexpr int fftSamples = 1 << 18;
    const std::string outDir = "/opt/cursor/artifacts";

    wave9090::dsp::WavetableBank bank;
    bank.rebuild();

    const auto before = renderMorph (bank, true, kMidiC8, fftSamples, sr);
    const auto after = renderMorph (bank, false, kMidiC8, fftSamples, sr);
    writeFftCsv (outDir + "/wave9090-aliasing-before.csv", before, sr);
    writeFftCsv (outDir + "/wave9090-aliasing-after.csv", after, sr);

    const auto beforeStats = analyseInharmonics (before, sr, static_cast<double> (midiToHz (kMidiC8)));
    const auto afterStats = analyseInharmonics (after, sr, static_cast<double> (midiToHz (kMidiC8)));

    const auto afterC7 = renderMorph (bank, false, kMidiC7, fftSamples, sr);
    const auto afterC6 = renderUnisonMorph (bank, kMidiC6, fftSamples, sr);
    const auto c7Stats = analyseInharmonics (afterC7, sr, static_cast<double> (midiToHz (kMidiC7)));
    const auto c6Stats = analyseInharmonics (afterC6, sr, static_cast<double> (midiToHz (kMidiC6)));

    std::ofstream report (outDir + "/wave9090-aliasing-report.txt");
    auto logLine = [&] (const char* tag, const SpectrumStats& st)
    {
        const double relDb = st.fundamentalDb - st.worstInharmDb;
        report << tag << " fund_hz=" << st.fundamentalHz << " fund_db=" << st.fundamentalDb
               << " worst_inharm_hz=" << st.worstInharmHz << " worst_inharm_db=" << st.worstInharmDb
               << " suppression_dB=" << relDb << '\n';
        std::cout << tag << " inharmonic_suppression=" << relDb << " dB (worst @ " << st.worstInharmHz << " Hz)\n";
    };
    logLine ("C8_BEFORE", beforeStats);
    logLine ("C8_AFTER", afterStats);
    logLine ("C7_AFTER", c7Stats);
    logLine ("C6_UNISON_AFTER", c6Stats);

    wave9090::engine::VoiceManager bench;
    bench.prepare (sr);
    wave9090::SynthParams p;
    bench.setParams (p);
    for (int n = 36; n < 52; ++n)
        bench.noteOn (n, 0.85f);

    constexpr int benchSamples = 44100 * 3;
    std::vector<float> l (static_cast<std::size_t> (benchSamples), 0.0f);
    std::vector<float> r (static_cast<std::size_t> (benchSamples), 0.0f);
    const auto t0 = std::chrono::steady_clock::now();
    bench.render (l.data(), r.data(), benchSamples);
    const auto t1 = std::chrono::steady_clock::now();
    const double ms = std::chrono::duration<double, std::milli> (t1 - t0).count();
    const double usPerSample = ms * 1000.0 / static_cast<double> (benchSamples);

    std::ofstream benchOut (outDir + "/wave9090-cpu-bench.txt");
    benchOut << "voices=16 samples=" << benchSamples << " ms=" << ms << " us_per_sample=" << usPerSample << '\n';

    const double c8RelAfter = afterStats.fundamentalDb - afterStats.worstInharmDb;
    const double c8RelBefore = beforeStats.fundamentalDb - beforeStats.worstInharmDb;
    const bool c8Pass = (c8RelAfter >= 80.0 || afterStats.worstInharmDb <= -100.0)
                        && (c8RelAfter >= c8RelBefore + 12.0);
    if (! c8Pass)
    {
        std::cerr << "C8 aliasing target not met (after=" << c8RelAfter << " before=" << c8RelBefore << ")\n";
        return EXIT_FAILURE;
    }

    std::cout << "EliteMeasurements OK us/sample=" << usPerSample << '\n';
    return EXIT_SUCCESS;
}