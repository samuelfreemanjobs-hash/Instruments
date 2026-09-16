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
constexpr int kZoneRoots[8] = { 28, 33, 40, 47, 54, 61, 68, 75 };

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

std::vector<float> buildByCategory (jdupgraded::assets::RomWaveCategory category,
                                    std::uint32_t seed,
                                    int zoneIndex,
                                    bool longLoop)
{
    const std::size_t cycleLen = 2048u + static_cast<std::size_t> ((seed % 4) * 512u);
    const std::size_t loopLen = cycleLen * (2u + (seed % 3u));
    const float zoneBright = 1.0f - static_cast<float> (zoneIndex) * 0.06f;

    switch (category)
    {
        case jdupgraded::assets::RomWaveCategory::analog:
            return makeAdditive (cycleLen, static_cast<int> (18 + zoneIndex),
                                 [zoneBright] (int h) { return zoneBright / static_cast<double> (h); });

        case jdupgraded::assets::RomWaveCategory::digital:
            return makeAdditive (cycleLen, 10,
                                 [seed, zoneBright] (int h)
                                 {
                                     return (h % (2 + seed % 5) == 0) ? zoneBright / h : 0.0;
                                 });

        case jdupgraded::assets::RomWaveCategory::bell:
            return makeBell (longLoop ? loopLen : cycleLen, 0.35f + static_cast<float> (seed % 7) * 0.08f);

        case jdupgraded::assets::RomWaveCategory::pluck:
            return makeKarplusStrong (longLoop ? loopLen : cycleLen,
                                      0.992f - static_cast<float> (zoneIndex) * 0.0015f,
                                      0.12f + zoneBright * 0.08f);

        case jdupgraded::assets::RomWaveCategory::bass:
            return makeAdditive (cycleLen, 12 + zoneIndex / 2,
                                 [zoneBright] (int h) { return zoneBright * std::exp (-h * 0.2) / h; });

        case jdupgraded::assets::RomWaveCategory::noise:
            return makeNoiseLoop (longLoop ? loopLen : cycleLen, 0.08f + static_cast<float> (seed % 8) * 0.015f);

        case jdupgraded::assets::RomWaveCategory::vocal:
            return makeVocalFormant (cycleLen,
                                     4.5f + static_cast<float> (zoneIndex),
                                     11.0f + static_cast<float> (seed % 5),
                                     20.0f + static_cast<float> (seed % 6));

        case jdupgraded::assets::RomWaveCategory::organ:
            return makeAdditive (cycleLen, 28,
                                 [zoneBright] (int h)
                                 {
                                     if (h % 2 == 0 || h % 3 == 0)
                                         return zoneBright * 0.55 / h;
                                     return 0.0;
                                 });

        case jdupgraded::assets::RomWaveCategory::fx:
        default:
        {
            auto wave = makeAdditive (longLoop ? loopLen : cycleLen, 18, [seed, zoneBright] (int h)
            {
                return zoneBright * std::sin (seed * 0.3 + h * 0.7) / static_cast<double> (h);
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

void assignEntry (jdupgraded::assets::RomWaveEntry& entry,
                  std::uint32_t id,
                  std::vector<float>& floats,
                  std::int16_t rootNote,
                  std::uint16_t multisampleSetId,
                  jdupgraded::assets::RomWaveCategory category)
{
    entry.waveId = id;
    entry.numFrames = static_cast<std::uint16_t> (std::min<std::size_t> (floats.size(), 65535));
    entry.category = static_cast<std::uint16_t> (category);
    entry.rootMidiNote = rootNote;
    entry.multisampleSetId = multisampleSetId;

    const bool isLoop = floats.size() > 3000;
    if (isLoop)
    {
        entry.flags = jdupgraded::assets::romWaveLooped;
        entry.loopStart = static_cast<std::uint16_t> (floats.size() / 8);
        entry.loopEnd = static_cast<std::uint16_t> (floats.size());
    }
    else
    {
        entry.flags = jdupgraded::assets::romWaveSingleCycle;
        entry.loopStart = 0;
        entry.loopEnd = entry.numFrames;
    }
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
    pcmPool.reserve (8 * 1024 * 1024);

    for (std::uint32_t id = 0; id < jdupgraded::assets::kSingleWaveCount; ++id)
    {
        auto& w = waves[id];
        const auto category = static_cast<jdupgraded::assets::RomWaveCategory> (id % 9);
        w.floats = buildByCategory (category, id, 0, id % 2 == 0);
        assignEntry (w.entry, id, w.floats,
                     static_cast<std::int16_t> (36 + (id % 36)),
                     0, category);
    }

    auto categoryForSet = [] (std::uint32_t set) -> jdupgraded::assets::RomWaveCategory
    {
        if (set <= 4)
            return jdupgraded::assets::RomWaveCategory::analog;
        if (set <= 7)
            return jdupgraded::assets::RomWaveCategory::digital;
        if (set <= 10)
            return jdupgraded::assets::RomWaveCategory::bell;
        if (set <= 13)
            return jdupgraded::assets::RomWaveCategory::pluck;
        if (set <= 16)
            return jdupgraded::assets::RomWaveCategory::bass;
        if (set <= 18)
            return jdupgraded::assets::RomWaveCategory::noise;
        if (set <= 20)
            return jdupgraded::assets::RomWaveCategory::vocal;
        if (set <= 22)
            return jdupgraded::assets::RomWaveCategory::organ;
        return jdupgraded::assets::RomWaveCategory::fx;
    };

    for (std::uint32_t set = 1; set <= jdupgraded::assets::kMultisampleSetCount; ++set)
    {
        const auto category = categoryForSet (set);
        for (std::uint32_t zone = 0; zone < jdupgraded::assets::kZonesPerSet; ++zone)
        {
            const std::uint32_t id = jdupgraded::assets::kSingleWaveCount + (set - 1) * jdupgraded::assets::kZonesPerSet + zone;
            auto& w = waves[id];
            w.floats = buildByCategory (category, set * 17 + zone, static_cast<int> (zone), true);
            assignEntry (w.entry, id, w.floats,
                         static_cast<std::int16_t> (kZoneRoots[zone]),
                         static_cast<std::uint16_t> (set), category);
        }
    }

    for (auto& w : waves)
    {
        w.entry.pcmOffsetBytes = static_cast<std::uint32_t> (pcmPool.size() * sizeof (std::int16_t));
        for (float sample : w.floats)
            pcmPool.push_back (floatToPcm16 (sample));
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
    std::printf ("Wrote %s (%zu bytes, %u waves, %u multisample sets)\n",
                 outPath, file.size(), waveCount, jdupgraded::assets::kMultisampleSetCount);
    return 0;
}
