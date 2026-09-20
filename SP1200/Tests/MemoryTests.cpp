#include "../Source/Memory/TwelveBitBuffer.h"
#include "../Source/Memory/SampleMemoryPool.h"
#include "../Source/SP1200Constants.h"

#include <cmath>
#include <iostream>

static bool expect (bool cond, const char* msg)
{
    if (! cond)
        std::cerr << "FAIL: " << msg << '\n';
    return cond;
}

int main()
{
    sp1200::TwelveBitBuffer buf;
    float mono[] = { 0.0f, 1.0f, -1.0f, 0.5f };
    buf.quantizeFromFloat (mono, 4);
    const auto packed = buf.pack();
    sp1200::TwelveBitBuffer restored;
    restored.unpack (packed.data(), packed.size(), 4);
    bool ok = true;
    for (std::size_t i = 0; i < 4; ++i)
    {
        const auto a = buf.getSample (i);
        const auto b = restored.getSample (i);
        ok &= expect (std::abs (static_cast<int> (a) - static_cast<int> (b)) <= 1, "pack/unpack");
    }

    sp1200::SampleMemoryPool pool;
    sp1200::SampleSegment seg;
    seg.data.resize (1000);
    ok &= expect (pool.appendSegment (std::move (seg)).has_value(), "append");
    ok &= expect (pool.usedSamples() == 1000, "used count");

    const auto tooMany = static_cast<std::size_t> (sp1200::kMaxMemorySamples) + 1;
    sp1200::SampleSegment big;
    big.data.resize (tooMany);
    ok &= expect (! pool.appendSegment (std::move (big)).has_value(), "7:00 cap");

    sp1200::SampleMemoryPool pool2;
    sp1200::SampleSegment a;
    a.bank = 0;
    a.data.resize (500);
    sp1200::SampleSegment b;
    b.bank = 1;
    b.data.resize (700);
    pool2.appendSegment (std::move (a));
    pool2.appendSegment (std::move (b));
    ok &= expect (pool2.usedSamplesInBank (0) == 500, "bank A used");
    ok &= expect (pool2.usedSamplesInBank (1) == 700, "bank B used");
    ok &= expect (pool2.usedSamplesInBank (2) == 0, "bank C empty");

    return ok ? 0 : 1;
}
