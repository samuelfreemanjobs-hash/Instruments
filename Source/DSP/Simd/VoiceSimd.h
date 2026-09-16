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

} // namespace jdupgraded::dsp
