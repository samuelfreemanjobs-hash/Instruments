#pragma once

// Real-time audio thread contract (Phase 1+).
// Anything invoked from AudioEngine::audioDeviceIOCallbackWithContext must obey these rules.

namespace vmpc::audio
{
// No heap allocation (new, malloc, std::vector::push_back, juce::String, etc.)
// No mutexes or blocking primitives on the audio thread
// No UI calls (Component, repaint, AsyncUpdater::trigger from audio thread — use atomics instead)
// No file I/O, logging, or exceptions in the hot path
// Prefer pre-allocated buffers and fixed-size structures

constexpr int kDefaultPpqn = 960;
} // namespace vmpc::audio
