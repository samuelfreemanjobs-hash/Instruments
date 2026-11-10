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

void addMinorChordHold (std::vector<std::pair<int, juce::MidiMessage>>& out,
                        double sr,
                        double bpm,
                        double startBeat,
                        double holdBeats,
                        const int notes[3],
                        int velocity)
{
    const int t0 = beatToSamples (startBeat, sr, bpm);
    const int t1 = beatToSamples (startBeat + holdBeats, sr, bpm);
    for (int i = 0; i < 3; ++i)
    {
        const int n = notes[i];
        addNote (out, t0, n, velocity, true);
        addNote (out, t1, n, 0, false);
    }
}

void addMelodyPhrase (std::vector<std::pair<int, juce::MidiMessage>>& out, double sr, double bpm, double startBeat)
{
    static const int melody[] = { 65, 68, 72, 76, 73, 72, 68, 67, 65, 72, 76, 77, 76, 72, 68, 65 };
    for (int i = 0; i < 16; ++i)
    {
        const double beat = startBeat + double (i) * 0.5;
        const int t0 = beatToSamples (beat, sr, bpm);
        const int t1 = beatToSamples (beat + 0.42, sr, bpm);
        addNote (out, t0, melody[i], 108, true);
        addNote (out, t1, melody[i], 0, false);
    }
}

void addStabs (std::vector<std::pair<int, juce::MidiMessage>>& out, double sr, double bpm)
{
    const int hits[] = { 58, 62, 65 };
    const double beats[] = { 0.25, 1.25, 2.5, 3.5, 4.75, 5.5 };
    for (double b : beats)
    {
        const int t0 = beatToSamples (b, sr, bpm);
        const int t1 = beatToSamples (b + 0.12, sr, bpm);
        for (int hi = 0; hi < 3; ++hi)
        {
            const int n = hits[hi];
            addNote (out, t0, n, 118, true);
            addNote (out, t1, n, 0, false);
        }
    }
}

void addBassHold (std::vector<std::pair<int, juce::MidiMessage>>& out, double sr, double bpm)
{
    const int note = 43; // G1 808 territory
    const int t0 = beatToSamples (0.5, sr, bpm);
    const int t1 = beatToSamples (5.5, sr, bpm);
    addNote (out, t0, note, 120, true);
    addNote (out, t1, note, 0, false);
}
} // namespace

std::vector<std::pair<int, juce::MidiMessage>> OfflineAudition::buildTimeline (const juce::String& category,
                                                                               bool includeSwellModWheel)
{
    std::vector<std::pair<int, juce::MidiMessage>> events;
    const double sr = kSampleRate;
    const double bpm = kBpm;
    const int fm[] = { 65, 68, 72 };

    if (category == "Pads" || category == "Swells" || category == "Strings" || category == "Choir")
        addMinorChordHold (events, sr, bpm, 0.5, 5.5, fm, category == "Swells" ? 92 : 98);
    else if (category == "Leads" || category == "Synth Keys" || category == "Church Keys/EP" || category == "Organ"
             || category == "Bells")
        addMelodyPhrase (events, sr, bpm, 1.0);
    else if (category == "Brass" || category == "Hits/FX")
        addStabs (events, sr, bpm);
    else if (category == "Bass")
        addBassHold (events, sr, bpm);
    else
        addMinorChordHold (events, sr, bpm, 0., 2.5, fm, 96);

    if (includeSwellModWheel)
    {
        const int totalSamples = int (std::round (kDurationSec * sr));
        const int steps = 32;
        for (int i = 0; i <= steps; ++i)
        {
            const int sample = int (int64_t (totalSamples) * i / steps);
            float t = float (i) / float (steps);
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
