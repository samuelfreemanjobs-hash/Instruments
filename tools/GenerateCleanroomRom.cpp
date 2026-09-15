#include "../Source/Assets/RomFormat.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <vector>

namespace
{

constexpr double kPi = 3.14159265358979323846;

float clamp (float v, float lo, float hi)
{
    return std::max (lo, std::min (hi, v));
}

void normalizePeak (std::vector<float>& buffer, float targetPeak = 0.92f)
{
    float peak = 0.0f;
    for (float s : buffer)
        peak = std::max (peak, std::abs (s));

    if (peak > 1.0e-6f)
    {
        const float gain = targetPeak / peak;
        for (float& s : buffer)
            s *= gain;
    }
}

std::vector<float> makeAdditive (std::size_t length, int maxHarmonic, auto harmonicGain)
{
    std::vector<float> out (length, 0.0f);
    for (std::size_t i = 0; i < length; ++i)
    {
        const double phase = static_cast<double> (i) / static_cast<double> (length);
        double sum = 0.0;
        for (int h = 1; h <= maxHarmonic; ++h)
            sum += harmonicGain (h) * std::sin (2.0 * kPi * phase * static_cast<double> (h));
        out[i] = static_cast<float> (sum);
    }
    normalizePeak (out);
    return out;
}

std::vector<float> makeKarplusStrong (std::size_t length, float decay, float brightness)
{
    std::vector<float> out (length, 0.0f);
    for (std::size_t i = 0; i < length / 8; ++i)
        out[i] = (static_cast<float> (std::rand()) / RAND_MAX) * 2.0f - 1.0f;

    for (std::size_t i = 1; i < length; ++i)
    {
        const float prev = out[i - 1];
        const float avg = 0.5f * (prev + out[(i > 1 ? i - 2 : length - 1)]);
        out[i] = avg * decay;
    }

    for (std::size_t i = 0; i < length; ++i)
        out[i] += brightness * (out[i] - out[(i + 1) % length]);

    normalizePeak (out);
    return out;
}

std::vector<float> makeBell (std::size_t length, float inharmonicity)
{
    std::vector<float> out (length, 0.0f);
    const double partials[] = { 1.0, 2.15, 3.2, 4.4, 5.7, 7.1 };
    const double gains[] = { 1.0, 0.55, 0.35, 0.22, 0.15, 0.1 };

    for (std::size_t i = 0; i < length; ++i)
    {
        const double t = static_cast<double> (i) / static_cast<double> (length);
        double sum = 0.0;
        for (int p = 0; p < 6; ++p)
        {
            const double freq = partials[p] * (1.0 + inharmonicity * 0.02 * static_cast<double> (p));
            sum += gains[p] * std::sin (2.0 * kPi * freq * t) * std::exp (-3.5 * t * (1.0 + 0.2 * p));
        }
        out[i] = static_cast<float> (sum);
    }
    normalizePeak (out);
    return out;
}

std::vector<float> makeNoiseLoop (std::size_t length, float lpCoeff)
{
    std::vector<float> out (length, 0.0f);
    float state = 0.0f;
    for (std::size_t i = 0; i < length; ++i)
    {
        const float white = (static_cast<float> (std::rand()) / RAND_MAX) * 2.0f - 1.0f;
        state += lpCoeff * (white - state);
        out[i] = state;
    }
    normalizePeak (out, 0.75f);
    return out;
}

std::vector<float> makeVocalFormant (std::size_t length, float f1, float f2, float f3)
{
    return makeAdditive (length, 40, [f1, f2, f3] (int h) -> double
    {
        const double f = static_cast<double> (h);
        const double env = std::exp (-f * 0.08);
        const double form = 1.0 / (1.0 + std::pow ((f - f1) / 2.5, 2.0))
                          + 0.7 / (1.0 + std::pow ((f - f2) / 3.0, 2.0))
                          + 0.4 / (1.0 + std::pow ((f - f3) / 4.0, 2.0));
        return env * form;
    });
}

std::vector<float> buildWave (std::uint32_t waveId)
{
    const std::size_t cycleLen = 2048u + static_cast<std::size_t> ((waveId % 4) * 512u);
    const std::size_t loopLen = cycleLen * (2u + (waveId % 3u));

    switch (static_cast<jdupgraded::assets::RomWaveCategory> (waveId % 9))
    {
        case jdupgraded::assets::RomWaveCategory::analog:
            return makeAdditive (cycleLen, 24 + static_cast<int> (waveId % 12),
                                 [] (int h) { return 1.0 / static_cast<double> (h); });

        case jdupgraded::assets::RomWaveCategory::digital:
            return makeAdditive (cycleLen, 8,
                                 [waveId] (int h)
                                 {
                                     return (h % (2 + waveId % 5) == 0) ? 1.0 / h : 0.0;
                                 });

        case jdupgraded::assets::RomWaveCategory::bell:
            return makeBell (loopLen, 0.4f + static_cast<float> (waveId % 7) * 0.1f);

        case jdupgraded::assets::RomWaveCategory::pluck:
            return makeKarplusStrong (loopLen, 0.992f - static_cast<float> (waveId % 5) * 0.002f, 0.15f);

        case jdupgraded::assets::RomWaveCategory::bass:
            return makeAdditive (cycleLen, 16,
                                 [] (int h) { return std::exp (-static_cast<double> (h) * 0.18) / h; });

        case jdupgraded::assets::RomWaveCategory::noise:
            return makeNoiseLoop (loopLen, 0.08f + static_cast<float> (waveId % 10) * 0.02f);

        case jdupgraded::assets::RomWaveCategory::vocal:
            return makeVocalFormant (cycleLen, 5.0f + static_cast<float> (waveId % 4),
                                     12.0f + static_cast<float> (waveId % 5),
                                     22.0f + static_cast<float> (waveId % 6));

        case jdupgraded::assets::RomWaveCategory::organ:
            return makeAdditive (cycleLen, 32,
                                 [] (int h)
                                 {
                                     if (h % 2 == 0 || h % 3 == 0)
                                         return 0.6 / h;
                                     return 0.0;
                                 });

        case jdupgraded::assets::RomWaveCategory::fx:
        default:
        {
            auto wave = makeAdditive (loopLen, 20, [waveId] (int h)
            {
                return std::sin (static_cast<double> (waveId) * 0.3 + h * 0.7) / static_cast<double> (h);
            });
            for (std::size_t i = 0; i < wave.size(); ++i)
                wave[i] *= std::sin (3.0 * kPi * static_cast<double> (i) / static_cast<double> (wave.size()));
            normalizePeak (wave);
            return wave;
        }
    }
}

std::int16_t floatToPcm16 (float sample)
{
    sample = clamp (sample, -1.0f, 1.0f);
    return static_cast<std::int16_t> (std::lround (sample * 32767.0f));
}

} // namespace

int main (int argc, char** argv)
{
    const char* outPath = argc > 1 ? argv[1] : "jdupg_cleanroom.rom";
    std::srand (0x4A445550u);

    struct WaveBuild
    {
        jdupgraded::assets::RomWaveEntry entry{};
        std::vector<float> floats;
    };

    std::vector<WaveBuild> waves;
    waves.resize (jdupgraded::assets::kCleanroomWaveCount);

    std::vector<std::int16_t> pcmPool;
    pcmPool.reserve (4 * 1024 * 1024);

    for (std::uint32_t id = 0; id < jdupgraded::assets::kCleanroomWaveCount; ++id)
    {
        auto& w = waves[id];
        w.floats = buildWave (id);
        w.entry.waveId = id;
        w.entry.numFrames = static_cast<std::uint16_t> (std::min<std::size_t> (w.floats.size(), 65535));
        w.entry.category = static_cast<std::uint16_t> (id % 9);
        w.entry.rootMidiNote = static_cast<std::int16_t> (36 + (id % 48));

        const bool isLoop = w.floats.size() > 3000;
        if (isLoop)
        {
            w.entry.flags = jdupgraded::assets::romWaveLooped;
            w.entry.loopStart = static_cast<std::uint16_t> (w.floats.size() / 8);
            w.entry.loopEnd = static_cast<std::uint16_t> (w.floats.size());
        }
        else
        {
            w.entry.flags = jdupgraded::assets::romWaveSingleCycle;
            w.entry.loopStart = 0;
            w.entry.loopEnd = w.entry.numFrames;
        }

        w.entry.pcmOffsetBytes = static_cast<std::uint32_t> (pcmPool.size() * sizeof (std::int16_t));
        for (std::size_t i = 0; i < w.floats.size(); ++i)
            pcmPool.push_back (floatToPcm16 (w.floats[static_cast<std::size_t> (i)]));
    }

    const std::uint32_t waveCount = static_cast<std::uint32_t> (waves.size());
    const std::uint32_t headerSize = static_cast<std::uint32_t> (sizeof (jdupgraded::assets::RomHeader));
    const std::uint32_t tableSize = waveCount * static_cast<std::uint32_t> (sizeof (jdupgraded::assets::RomWaveEntry));
    const std::uint32_t pcmOffset = headerSize + tableSize;
    const std::uint32_t pcmBytes = static_cast<std::uint32_t> (pcmPool.size() * sizeof (std::int16_t));

    jdupgraded::assets::RomHeader header{};
    std::memcpy (header.magic, jdupgraded::assets::kRomMagic, sizeof (header.magic));
    header.version = jdupgraded::assets::kRomVersion;
    header.waveCount = waveCount;
    header.waveTableOffset = headerSize;
    header.pcmDataOffset = pcmOffset;
    header.pcmBytes = pcmBytes;
    header.crc32 = 0;

    std::vector<std::uint8_t> file;
    file.resize (pcmOffset + pcmBytes);

    std::memcpy (file.data(), &header, sizeof (header));
    for (std::uint32_t i = 0; i < waveCount; ++i)
    {
        const auto offset = header.waveTableOffset + i * sizeof (jdupgraded::assets::RomWaveEntry);
        std::memcpy (file.data() + offset, &waves[i].entry, sizeof (jdupgraded::assets::RomWaveEntry));
    }
    std::memcpy (file.data() + pcmOffset, pcmPool.data(), pcmBytes);

    header.crc32 = jdupgraded::assets::crc32 (file.data() + pcmOffset, pcmBytes);
    std::memcpy (file.data(), &header, sizeof (header));

    std::ofstream out (outPath, std::ios::binary);
    if (! out)
    {
        std::fprintf (stderr, "Failed to open %s for writing\n", outPath);
        return 1;
    }
    out.write (reinterpret_cast<const char*> (file.data()), static_cast<std::streamsize> (file.size()));
    std::printf ("Wrote %s (%zu bytes, %u waves)\n", outPath, file.size(), waveCount);
    return 0;
}
