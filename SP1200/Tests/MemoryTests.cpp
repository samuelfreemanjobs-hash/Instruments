#include "../Source/Memory/TwelveBitBuffer.h"
#include "../Source/Memory/SampleMemoryPool.h"
#include "../Source/Engine/PadAssignments.h"
#include "../Source/Engine/SegmentEditor.h"
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

    sp1200::SampleMemoryPool pool3;
    sp1200::SampleSegment s1;
    s1.data.resize (100);
    s1.data.setSample (0, 100);
    s1.data.setSample (99, 200);
    sp1200::SampleSegment s2;
    s2.data.resize (50);
    s2.data.setSample (0, 300);
    pool3.appendSegment (std::move (s1));
    pool3.appendSegment (std::move (s2));

    sp1200::PadBank pads {};
    pads.pads[0].segmentIndex = 0;
    pads.pads[1].segmentIndex = 1;
    ok &= expect (sp1200::combineSegments (pool3, pads, 0, 1), "combine");
    ok &= expect (pool3.segmentCount() == 1, "one segment after combine");
    ok &= expect (pool3.usedSamples() == 150, "combined length");
    const auto* combined = pool3.getSegment (0);
    ok &= expect (combined != nullptr && combined->data.size() == 150, "combined size");
    ok &= expect (combined->data.getSample (0) == 100 && combined->data.getSample (100) == 300, "combined data");
    ok &= expect (pads.pads[0].segmentIndex == 0 && pads.pads[1].segmentIndex == 0, "pads remapped");

    sp1200::SampleSegment s3;
    s3.bank = 2;
    s3.data.resize (10);
    pool3.appendSegment (std::move (s3));
    ok &= expect (sp1200::setSegmentBank (pool3, 1, 1), "set bank");
    ok &= expect (pool3.getSegment (1)->bank == 1, "bank tag");

    return ok ? 0 : 1;
}
