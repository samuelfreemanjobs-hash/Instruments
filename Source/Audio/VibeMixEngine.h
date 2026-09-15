#pragma once

#include "VibeMixingTypes.h"

namespace vmpc::audio
{
/**
 * Vibe Mixing — turn a typed brief into an internal plug-in chain + preset.
 * Keyword/heuristic engine today; structured for future ML/LLM backends.
 */
class VibeMixEngine
{
public:
    static VibeMixInterpretResult interpretUserBrief(const juce::String& userText);
};
} // namespace vmpc::audio
