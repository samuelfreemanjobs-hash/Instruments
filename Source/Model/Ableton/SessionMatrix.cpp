#include "SessionMatrix.h"

namespace vmpc::model
{
void SessionMatrix::launchScene(int sceneIndex)
{
    const int scene = juce::jlimit(0, kScenes - 1, sceneIndex);
    for (int t = 0; t < kTracks; ++t)
    {
        auto& c = clip(t, scene);
        if (c.hasClip)
            c.isPlaying = true;
    }
}
} // namespace vmpc::model
