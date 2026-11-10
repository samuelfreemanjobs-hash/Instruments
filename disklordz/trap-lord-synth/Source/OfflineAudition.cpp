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

void addChordHold (std::vector<std::pair<int, juce::MidiMessage>>& out,
                   double sr,
                   double bpm,
                   double startBeat,
                   double holdBeats,
                   int root,
                   const int intervals[],
                   int intervalCount,
                   int velocity)
{
    const int t0 = beatToSamples (startBeat, sr, bpm);
    const int t1 = beatToSamples (startBeat + holdBeats, sr, bpm);
    for (int i = 0; i < intervalCount; ++i)
    {
        const int n = root + intervals[i];
        addNote (out, t0, n, velocity, true);
        addNote (out, t1, n, 0, false);
    }
}

void addMelodyPhrase (std::vector<std::pair<int, juce::MidiMessage>>& out,
                      double sr,
                      double bpm,
                      double startBeat,
                      int transpose,
                      double stepBeats)
{
    static const int melody[] = { 65, 68, 72, 76, 73, 72, 68, 67, 65, 72, 76, 77, 76, 72, 68, 65 };
    for (int i = 0; i < 16; ++i)
    {
        const double beat = startBeat + double (i) * stepBeats;
        const int t0 = beatToSamples (beat, sr, bpm);
        const int t1 = beatToSamples (beat + juce::jmin (0.42, stepBeats * 0.9), sr, bpm);
        const int note = juce::jlimit (36, 96, melody[i] + transpose);
        addNote (out, t0, note, 108, true);
        addNote (out, t1, note, 0, false);
    }
}

void addStabs (std::vector<std::pair<int, juce::MidiMessage>>& out, double sr, double bpm, int root, int variant)
{
    const int hits[] = { root, root + 4, root + 7 };
    const double patterns[][6] = {
        { 0.25, 1.25, 2.5, 3.5, 4.75, 5.5 },
        { 0.5, 1.0, 2.0, 3.25, 4.0, 5.25 },
        { 1.0, 1.75, 2.75, 3.75, 4.5, 5.75 }
    };
    const auto& beats = patterns[juce::jlimit (0, 2, variant % 3)];
    const double gate = variant == 2 ? 0.18 : 0.12;
    for (double b : beats)
    {
        const int t0 = beatToSamples (b, sr, bpm);
        const int t1 = beatToSamples (b + gate, sr, bpm);
        for (int n : hits)
        {
            addNote (out, t0, n, 118, true);
            addNote (out, t1, n, 0, false);
        }
    }
}

void addBassHold (std::vector<std::pair<int, juce::MidiMessage>>& out, double sr, double bpm, int note)
{
    const int t0 = beatToSamples (0.5, sr, bpm);
    const int t1 = beatToSamples (5.5, sr, bpm);
    addNote (out, t0, note, 120, true);
    addNote (out, t1, note, 0, false);
}
} // namespace

std::vector<std::pair<int, juce::MidiMessage>> OfflineAudition::buildTimeline (const juce::String& category,
                                                                               bool includeSwellModWheel,
                                                                               int phraseVariant)
{
    std::vector<std::pair<int, juce::MidiMessage>> events;
    const double sr = kSampleRate;
    const double bpm = kBpm;
    const int v = juce::jlimit (0, 5, phraseVariant);

    if (category == "Pads" || category == "Swells" || category == "Strings" || category == "Choir")
    {
        const int roots[] = { 60, 62, 65, 57, 53, 48 };
        static const int minor[] = { 0, 3, 7 };
        static const int min7[] = { 0, 3, 7, 10 };
        static const int sus2[] = { 0, 2, 7 };
        static const int open4[] = { 0, 5, 7 };
        const int* intervals = minor;
        int icount = 3;
        if (v % 3 == 1)
        {
            intervals = min7;
            icount = 4;
        }
        else if (v % 3 == 2)
        {
            intervals = sus2;
            icount = 3;
        }
        else if (v == 4)
        {
            intervals = open4;
            icount = 3;
        }
        addChordHold (events, sr, bpm, 0.35 + v * 0.05, 5.2 - v * 0.08, roots[v], intervals, icount,
                      category == "Swells" ? 90 : 98);
    }
    else if (category == "Leads" || category == "Synth Keys" || category == "Church Keys/EP" || category == "Organ"
             || category == "Bells")
    {
        const int transpose = (v - 2) * 2;
        const double step = v % 2 == 0 ? 0.5 : 0.33;
        addMelodyPhrase (events, sr, bpm, 0.75 + v * 0.08, transpose, step);
    }
    else if (category == "Brass" || category == "Hits/FX")
    {
        const int roots[] = { 58, 55, 60, 53, 62, 50 };
        addStabs (events, sr, bpm, roots[v], v);
    }
    else if (category == "Bass")
    {
        const int roots[] = { 43, 40, 36, 41, 38, 45 };
        addBassHold (events, sr, bpm, roots[v]);
    }
    else
    {
        static const int minor[] = { 0, 3, 7 };
        addChordHold (events, sr, bpm, 0., 2.5, 60, minor, 3, 96);
    }

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
