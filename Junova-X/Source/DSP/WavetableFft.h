#pragma once

#include <array>
#include <cmath>
#include <complex>
#include <cstddef>
#include <vector>

namespace junovax::dsp
{
inline void fftRadix2 (std::vector<std::complex<double>>& data)
{
    const std::size_t n = data.size();
    std::size_t j = 0;
    for (std::size_t i = 1; i < n; ++i)
    {
        std::size_t bit = n >> 1;
        for (; j & bit; bit >>= 1)
            j ^= bit;
        j ^= bit;
        if (i < j)
            std::swap (data[i], data[j]);
    }

    for (std::size_t len = 2; len <= n; len <<= 1)
    {
        const double ang = -2.0 * 3.141592653589793 / static_cast<double> (len);
        const std::complex<double> wlen (std::cos (ang), std::sin (ang));
        for (std::size_t i = 0; i < n; i += len)
        {
            std::complex<double> w (1.0, 0.0);
            for (std::size_t k = 0; k < len / 2; ++k)
            {
                const auto u = data[i + k];
                const auto v = data[i + k + len / 2] * w;
                data[i + k] = u + v;
                data[i + k + len / 2] = u - v;
                w *= wlen;
            }
        }
    }
}

inline void ifftRadix2 (std::vector<std::complex<double>>& data)
{
    for (auto& c : data)
        c = std::conj (c);
    fftRadix2 (data);
    const double inv = 1.0 / static_cast<double> (data.size());
    for (auto& c : data)
        c = std::conj (c) * inv;
}

template <std::size_t N>
void bandLimitCycle (const std::array<float, N>& timeIn, int maxHarmonic, std::array<float, N>& timeOut)
{
    std::vector<std::complex<double>> spec (N);
    for (std::size_t i = 0; i < N; ++i)
        spec[i] = { static_cast<double> (timeIn[i]), 0.0 };

    fftRadix2 (spec);

    const int maxBin = std::min (static_cast<int> (N / 2), std::max (1, maxHarmonic));
    for (std::size_t k = 1; k < N / 2; ++k)
    {
        if (static_cast<int> (k) > maxBin)
        {
            spec[k] = { 0.0, 0.0 };
            spec[N - k] = { 0.0, 0.0 };
        }
    }
    spec[0].imag (0.0);
    spec[N / 2].imag (0.0);

    ifftRadix2 (spec);
    for (std::size_t i = 0; i < N; ++i)
        timeOut[i] = static_cast<float> (spec[i].real());
}
} // namespace junovax::dsp