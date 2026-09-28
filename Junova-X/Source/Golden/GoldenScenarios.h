#pragma once

class JunovaXAudioProcessor;

namespace junovax::golden
{
/** Apply competitive A/B patch (COMPETITIVE_JUN6.md). Returns false if id unknown. */
bool applyScenario (JunovaXAudioProcessor& processor, const char* scenarioId) noexcept;

const char* listScenarioIds() noexcept;
} // namespace junovax::golden
