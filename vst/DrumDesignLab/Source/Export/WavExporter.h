#pragma once
// Minimal PCM WAV writer + QA helpers (portable, no JUCE).
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

namespace disklordz::export_wav
{

struct QaReport
{
    bool ok = true;
    bool hasNan = false;
    bool silent = false;
    bool clipped = false;
    float peak = 0.f;
    float dc = 0.f;
};

inline QaReport analyzeInterleavedStereo(const float* lr, int frames) noexcept
{
    QaReport r;
    if (frames <= 0)
    {
        r.ok = false;
        r.silent = true;
        return r;
    }
    double sum = 0.0;
    int nonzero = 0;
    for (int i = 0; i < frames * 2; ++i)
    {
        const float v = lr[i];
        if (!std::isfinite(v))
        {
            r.hasNan = true;
            r.ok = false;
            continue;
        }
        const float a = std::fabs(v);
        r.peak = std::max(r.peak, a);
        sum += static_cast<double>(v);
        if (a > 1e-5f)
            ++nonzero;
        if (a >= 0.999f)
            r.clipped = true;
    }
    r.dc = static_cast<float>(sum / static_cast<double>(frames * 2));
    r.silent = nonzero < std::max(16, frames / 100);
    if (r.silent || r.hasNan || std::fabs(r.dc) > 0.08f)
        r.ok = false;
    return r;
}

inline bool writeWav16Stereo(const std::string& path, const float* interleavedLR, int frames,
                             int sampleRate) noexcept
{
    if (frames <= 0 || interleavedLR == nullptr)
        return false;
    FILE* f = nullptr;
#if defined(_MSC_VER)
    if (fopen_s(&f, path.c_str(), "wb") != 0 || f == nullptr)
        return false;
#else
    f = std::fopen(path.c_str(), "wb");
    if (f == nullptr)
        return false;
#endif
    const int dataBytes = frames * 2 * 2;
    const int byteRate = sampleRate * 2 * 2;
    auto w32 = [&](uint32_t v) {
        unsigned char b[4] = { static_cast<unsigned char>(v & 255),
                               static_cast<unsigned char>((v >> 8) & 255),
                               static_cast<unsigned char>((v >> 16) & 255),
                               static_cast<unsigned char>((v >> 24) & 255) };
        std::fwrite(b, 1, 4, f);
    };
    auto w16 = [&](uint16_t v) {
        unsigned char b[2] = { static_cast<unsigned char>(v & 255),
                               static_cast<unsigned char>((v >> 8) & 255) };
        std::fwrite(b, 1, 2, f);
    };

    std::fwrite("RIFF", 1, 4, f);
    w32(static_cast<uint32_t>(36 + dataBytes));
    std::fwrite("WAVE", 1, 4, f);
    std::fwrite("fmt ", 1, 4, f);
    w32(16);
    w16(1);
    w16(2);
    w32(static_cast<uint32_t>(sampleRate));
    w32(static_cast<uint32_t>(byteRate));
    w16(4);
    w16(16);
    std::fwrite("data", 1, 4, f);
    w32(static_cast<uint32_t>(dataBytes));

    for (int i = 0; i < frames * 2; ++i)
    {
        float x = std::clamp(interleavedLR[i], -1.0f, 1.0f);
        const int16_t s = static_cast<int16_t>(std::lrintf(x * 32767.0f));
        w16(static_cast<uint16_t>(s));
    }
    std::fclose(f);
    return true;
}

} // namespace disklordz::export_wav
