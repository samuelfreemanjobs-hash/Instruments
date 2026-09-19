#pragma once

#include <cstddef>
#include <cstdint>

namespace disklordz::rompler::content
{

/** Disk streaming policy — implement when ROM exceeds RAM budget. */
struct StreamingPolicy final
{
    std::uint32_t preloadMilliseconds = 512;
    std::uint32_t streamingThresholdBytes = 65536;
    std::size_t cacheBudgetBytes = 256u * 1024u * 1024u;
};

/**
 * Placeholder cache API (P0: full ROM cards resident via RawRomLibrary).
 * Future: zone-keyed PCM windows + background prefetch.
 */
class StreamingSampleCache final
{
public:
    void setPolicy (StreamingPolicy p) noexcept { policy_ = p; }
    StreamingPolicy getPolicy() const noexcept { return policy_; }

    /** Returns true if sample tail should stream from disk instead of resident pool. */
    bool shouldStream (std::uint32_t zonePcmBytes) const noexcept
    {
        return zonePcmBytes > policy_.streamingThresholdBytes;
    }

private:
    StreamingPolicy policy_{};
};

} // namespace disklordz::rompler::content
