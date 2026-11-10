#include "OfflineAudition.h"
#include <cmath>

namespace
{
int beatToSamples (double beat, double sr, double bpm)
{
    return int (std::round (beat * sr * 60. / bpm));
}

void addNote (std::vector<std::pair<int, juce::MidiMessage>>& out, int sample, int note, int vel, bool on)
{
    out.emplace_back (sample, on ? juce::MidiMessage::noteOn (1, note, (juce::uint8) juce::jlimit (1, 127, vel))
                                 : juce::MidiMessage::noteOff (1, note));
}

void addModWheel (std::vector<std::pair<int, juce::MidiMessage>>& out, int sample, int value)
{
    out.emplace_back (sample, juce::MidiMessage::controllerEvent (1, 1, (juce::uint8) juce::jlimit (0, 127, value)));
}
} // namespace

std::vector<std::pair<int, juce::MidiMessage>> OfflineAudition::buildTimeline (bool includeSwellModWheel)
{
    std::vector<std::pair<int, juce::MidiMessage>> events;
    const double sr = kSampleRate;
    const double bpm = kBpm;

    // Short F-minor chord phrase (staccato quarters on beats 0–1.5).
    const int fm[] = { 65, 68, 72 };
    const double stabBeats[] = { 0., .5, 1., 1.5 };
    for (double b : stabBeats)
    {
        const int t0 = beatToSamples (b, sr, bpm);
        const int t1 = beatToSamples (b + 0.35, sr, bpm);
        for (int n : fm)
        {
            addNote (events, t0, n, 98, true);
            addNote (events, t1, n, 0, false);
        }
    }

    // Melody starts at beat 2 (after phrase).
    static const int melody[] = { 65, 68, 72, 76, 73, 72, 68, 67, 65, 72, 76, 77, 76, 72, 68, 65 };
    const double melodyStartBeat = 2.;
    for (int i = 0; i < 16; ++i)
    {
        const double beat = melodyStartBeat + double (i) * 0.5;
        const int t0 = beatToSamples (beat, sr, bpm);
        const int t1 = beatToSamples (beat + 0.45, sr, bpm);
        addNote (events, t0, melody[i], 108, true);
        addNote (events, t1, melody[i], 0, false);
    }

    if (includeSwellModWheel)
    {
        const int totalSamples = int (std::round (kDurationSec * sr));
        const int steps = 32;
        for (int i = 0; i <= steps; ++i)
        {
            const int sample = int (int64_t (totalSamples) * i / steps);
            float t = float (i) / float (steps);
            // Slow rise, hold, gentle fall — audible swell on mod-wheel patches.
            float shaped = t < 0.55f ? t / 0.55f : 1.f - (t - 0.55f) / 0.45f * 0.35f;
            const int cc = int (std::round (shaped * 127.f));
            addModWheel (events, sample, cc);
        }
    }

    std::sort (events.begin(), events.end(), [] (const auto& a, const auto& b) { return a.first < b.first; });
    return events;
}

juce::String OfflineAudition::safeFilename (const juce::String& category, const juce::String& presetName, int slot)
{
    auto slug = category + "_" + presetName + "_" + juce::String (slot);
    slug = slug.replaceCharacters (" /\\:*?\"<>|", "___________");
    while (slug.contains ("__"))
        slug = slug.replace ("__", "_");
    return slug.trim() + ".wav";
}
