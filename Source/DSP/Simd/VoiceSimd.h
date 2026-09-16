#pragma once

#include "DSP/AudioConstants.h"

#include <array>
#include <cstddef>

#if defined(JDUPGRADED_USE_AVX2)
#include <immintrin.h>
#endif

#if defined(JDUPGRADED_USE_NEON)
#include <arm_neon.h>
#endif

namespace jdupgraded::dsp
{

/** Sum four tone buffers into `dest`. */
inline void sumFourToneBuffers (float* dest,
                                const float* tone0,
                                const float* tone1,
                                const float* tone2,
                                const float* tone3,
                                std::size_t numSamples) noexcept
{
#if defined(JDUPGRADED_USE_AVX2)
    std::size_t i = 0;
    for (; i + 8 <= numSamples; i += 8)
    {
        const auto a = _mm256_loadu_ps (tone0 + i);
        const auto b = _mm256_loadu_ps (tone1 + i);
        const auto c = _mm256_loadu_ps (tone2 + i);
        const auto d = _mm256_loadu_ps (tone3 + i);
        const auto sum = _mm256_add_ps (_mm256_add_ps (a, b), _mm256_add_ps (c, d));
        _mm256_storeu_ps (dest + i, sum);
    }
    for (; i < numSamples; ++i)
        dest[i] = tone0[i] + tone1[i] + tone2[i] + tone3[i];
#elif defined(JDUPGRADED_USE_NEON)
    std::size_t i = 0;
    for (; i + 4 <= numSamples; i += 4)
    {
        const auto a = vld1q_f32 (tone0 + i);
        const auto b = vld1q_f32 (tone1 + i);
        const auto c = vld1q_f32 (tone2 + i);
        const auto d = vld1q_f32 (tone3 + i);
        const auto sum = vaddq_f32 (vaddq_f32 (a, b), vaddq_f32 (c, d));
        vst1q_f32 (dest + i, sum);
    }
    for (; i < numSamples; ++i)
        dest[i] = tone0[i] + tone1[i] + tone2[i] + tone3[i];
#else
    for (std::size_t i = 0; i < numSamples; ++i)
        dest[i] = tone0[i] + tone1[i] + tone2[i] + tone3[i];
#endif
}

inline void accumulateFourToneBuffers (float* dest,
                                       const std::array<const float*, kTonesPerVoice>& tones,
                                       std::size_t numSamples) noexcept
{
    sumFourToneBuffers (dest, tones[0], tones[1], tones[2], tones[3], numSamples);
}

/** `dest[i] += src[i]`. */
inline void addBuffers (float* dest, const float* src, std::size_t numSamples) noexcept
{
#if defined(JDUPGRADED_USE_AVX2)
    std::size_t i = 0;
    for (; i + 8 <= numSamples; i += 8)
    {
        const auto d = _mm256_loadu_ps (dest + i);
        const auto s = _mm256_loadu_ps (src + i);
        _mm256_storeu_ps (dest + i, _mm256_add_ps (d, s));
    }
    for (; i < numSamples; ++i)
        dest[i] += src[i];
#elif defined(JDUPGRADED_USE_NEON)
    std::size_t i = 0;
    for (; i + 4 <= numSamples; i += 4)
    {
        const auto d = vld1q_f32 (dest + i);
        const auto s = vld1q_f32 (src + i);
        vst1q_f32 (dest + i, vaddq_f32 (d, s));
    }
    for (; i < numSamples; ++i)
        dest[i] += src[i];
#else
    for (std::size_t i = 0; i < numSamples; ++i)
        dest[i] += src[i];
#endif
}

inline void scaleBuffer (float* dest, float gain, std::size_t numSamples) noexcept
{
#if defined(JDUPGRADED_USE_AVX2)
    const auto g = _mm256_set1_ps (gain);
    std::size_t i = 0;
    for (; i + 8 <= numSamples; i += 8)
    {
        const auto v = _mm256_loadu_ps (dest + i);
        _mm256_storeu_ps (dest + i, _mm256_mul_ps (v, g));
    }
    for (; i < numSamples; ++i)
        dest[i] *= gain;
#elif defined(JDUPGRADED_USE_NEON)
    const auto g = vdupq_n_f32 (gain);
    std::size_t i = 0;
    for (; i + 4 <= numSamples; i += 4)
    {
        const auto v = vld1q_f32 (dest + i);
        vst1q_f32 (dest + i, vmulq_f32 (v, g));
    }
    for (; i < numSamples; ++i)
        dest[i] *= gain;
#else
    for (std::size_t i = 0; i < numSamples; ++i)
        dest[i] *= gain;
#endif
}

} // namespace jdupgraded::dsp
