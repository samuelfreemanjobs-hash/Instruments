#pragma once

#include <JuceHeader.h>
#include <array>

namespace vmpc::model
{
/** Maschine-inspired: Groups (A–H), Scenes, 16 pads per group. */
struct MaschinePad
{
    bool hasSample = false;
    juce::String sampleId;
    int chokeGroup = 0;
};

struct MaschineGroup
{
    juce::String name { "GROUP" };
    std::array<MaschinePad, 16> pads {};
};

struct MaschineScene
{
    juce::String name { "SCENE" };
    std::array<int, 8> patternIndexPerGroup {}; // pattern slot per group
};

class MaschineKit
{
public:
    static constexpr int kGroups = 8;
    static constexpr int kScenes = 16;

    MaschineGroup& getGroup(int i) noexcept { return groups[static_cast<size_t>(i & 7)]; }
    const MaschineGroup& getGroup(int i) const noexcept { return groups[static_cast<size_t>(i & 7)]; }
    MaschineScene& getScene(int i) noexcept { return scenes[static_cast<size_t>(i & 15)]; }

    int getActiveGroup() const noexcept { return activeGroup; }
    void setActiveGroup(int g) noexcept { activeGroup = juce::jlimit(0, kGroups - 1, g); }

    int getActiveScene() const noexcept { return activeScene; }
    void setActiveScene(int s) noexcept { activeScene = juce::jlimit(0, kScenes - 1, s); }

    bool isKeyboardMode() const noexcept { return keyboardMode; }
    void setKeyboardMode(bool on) noexcept { keyboardMode = on; }

private:
    std::array<MaschineGroup, kGroups> groups {};
    std::array<MaschineScene, kScenes> scenes {};
    int activeGroup = 0;
    int activeScene = 0;
    bool keyboardMode = false;
};
} // namespace vmpc::model
