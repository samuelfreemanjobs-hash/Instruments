#pragma once

#include "Assets/SampleBank.h"
#include "Engine/RomplerParams.h"
#include "Engine/RomplerVoice.h"

#include <array>
#include <cmath>

namespace disklordz::rompler::engine
{

class RomplerEngine final
{
public:
    static constexpr int kMaxVoices = 24;

    void setSampleBank (const assets::SampleBank* bank) noexcept
    {
        bank_ = bank;
    }

    void prepare (double sampleRate) noexcept
    {
        sampleRate_ = sampleRate;
        for (auto& v : voices_)
            v.prepare (sampleRate, bank_);
    }

    void reset() noexcept
    {
        for (auto& v : voices_)
            v.reset();
    }

    void setParams (const RomplerParams& p) noexcept
    {
        params_ = p;
        for (auto& v : voices_)
            if (v.isActive())
                v.setParams (params_);
    }

    void noteOn (int note, float velocity) noexcept
    {
        if (note < 0 || note > 127)
            return;

        keyHeld_[static_cast<std::size_t> (note)] = true;
        sustained_[static_cast<std::size_t> (note)] = false;

        auto* v = findFreeVoice();
        if (v == nullptr)
            v = stealVoice();
        v->start (note, velocity, params_);
    }

    void noteOff (int note) noexcept
    {
        if (note < 0 || note > 127)
            return;

        keyHeld_[static_cast<std::size_t> (note)] = false;
        if (sustainPedal_)
        {
            sustained_[static_cast<std::size_t> (note)] = true;
            return;
        }
        releaseNote (note);
    }

    void setSustainPedal (bool down) noexcept
    {
        if (sustainPedal_ == down)
            return;
        sustainPedal_ = down;
        if (! sustainPedal_)
        {
            for (int n = 0; n < 128; ++n)
            {
                if (sustained_[static_cast<std::size_t> (n)] && ! keyHeld_[static_cast<std::size_t> (n)])
                    releaseNote (n);
                sustained_[static_cast<std::size_t> (n)] = false;
            }
        }
    }

    void setPitchBendSemis (float semis) noexcept
    {
        params_.pitchBendSemis = semis;
        setParams (params_);
    }

    void render (float* outL, float* outR, int numSamples) noexcept
    {
        for (int i = 0; i < numSamples; ++i)
        {
            float l = 0.0f;
            float r = 0.0f;
            for (auto& v : voices_)
                v.render (l, r);
            outL[i] += l;
            outR[i] += r;
        }
    }

private:
    RomplerVoice* findFreeVoice() noexcept
    {
        for (auto& v : voices_)
            if (! v.isActive())
                return &v;
        return nullptr;
    }

    RomplerVoice* stealVoice() noexcept
    {
        stealCursor_ = (stealCursor_ + 1) % kMaxVoices;
        return &voices_[stealCursor_];
    }

    void releaseNote (int note) noexcept
    {
        for (auto& v : voices_)
            if (v.isActive() && v.getNote() == note)
                v.stop();
    }

    double sampleRate_ = 44100.0;
    const assets::SampleBank* bank_ = nullptr;
    RomplerParams params_{};
    std::array<RomplerVoice, kMaxVoices> voices_{};
    std::array<bool, 128> keyHeld_{};
    std::array<bool, 128> sustained_{};
    bool sustainPedal_ = false;
    std::size_t stealCursor_ = 0;
};

} // namespace disklordz::rompler::engine
