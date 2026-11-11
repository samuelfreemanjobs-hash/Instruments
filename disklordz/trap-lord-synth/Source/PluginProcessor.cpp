#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PresetLibrary.h"
#include "MpcXpmWriter.h"
#include "BinaryData.h"
#include <cmath>

namespace
{
using Layout = juce::AudioProcessorValueTreeState::ParameterLayout;

bool writeKeygroupWav (const juce::File& file, const juce::AudioBuffer<float>& buffer, double sampleRate, int bits)
{
    file.getParentDirectory().createDirectory();
    file.deleteFile();

    juce::AudioFormatWriterOptions opts;
    opts = opts.withSampleRate (sampleRate)
             .withNumChannels (buffer.getNumChannels())
             .withBitsPerSample (bits)
             .withSampleFormat (juce::AudioFormatWriterOptions::SampleFormat::integral);

    juce::WavAudioFormat wav;
    std::unique_ptr<juce::OutputStream> stream (file.createOutputStream());
    if (! stream)
        return false;

    std::unique_ptr<juce::AudioFormatWriter> writer (wav.createWriterFor (stream, opts));
    if (! writer)
        return false;

  return writer->writeFromAudioSampleBuffer (buffer, 0, buffer.getNumSamples());
}

void param (Layout& l, const char* id, const char* label, float lo, float hi, float def)
{
    l.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { id, 1 }, label, juce::NormalisableRange<float> (lo, hi), def));
}

float get (const juce::AudioProcessorValueTreeState& s, const char* id, float fallback = 0.f)
{
    if (auto* p = s.getRawParameterValue (id))
        return p->load();
    return fallback;
}
} // namespace

TrapLordSynthAudioProcessor::TrapLordSynthAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      state (*this, nullptr, "PARAMETERS", makeLayout())
{
    synth.addSound (new TrapSound());
    for (int i = 0; i < 16; ++i)
        synth.addVoice (new TrapVoice());
    bank = std::make_shared<const std::vector<TrapSamplePtr>>();
    PresetLibrary::instance().loadFromBinaryData (BinaryData::preset_bank_json,
                                                  BinaryData::preset_bank_jsonSize);
}

Layout TrapLordSynthAudioProcessor::makeLayout()
{
    Layout l;
    param (l, "attack", "Attack", .001f, 5.f, .01f);
    param (l, "decay", "Decay", .001f, 5.f, .3f);
    param (l, "sustain", "Sustain", 0.f, 1.f, .7f);
    param (l, "release", "Release", .01f, 8.f, .4f);
    param (l, "cutoff", "Cutoff", 20.f, 18000.f, 7000.f);
    param (l, "gain", "Gain", 0.f, .7f, .2f);
    param (l, "drive", "Drive", .1f, 8.f, 1.f);
    param (l, "detune", "Detune cents", 0.f, 40.f, 7.f);
    param (l, "reverb", "Reverb mix", 0.f, 1.f, .15f);
    param (l, "delay", "Delay mix", 0.f, .7f, 0.f);
    param (l, "arpRate", "Arp division", 1.f, 16.f, 4.f);
    param (l, "arpGate", "Arp gate", .1f, .95f, .65f);
    param (l, "resonance", "Resonance", 0.f, 1.f, 0.55f);
    param (l, "pitchEnvSemi", "Pitch env semi", 0.f, 12.f, 0.f);
    param (l, "pitchEnvMs", "Pitch env ms", 1.f, 500.f, 45.f);
    param (l, "pulseWidth", "Pulse width", 0.05f, 0.95f, 0.5f);
    param (l, "portamentoMs", "Portamento ms", 0.f, 800.f, 0.f);
    param (l, "vibratoDepth", "Vibrato depth", 0.f, 1.f, 0.f);
    param (l, "swellDepth", "Swell depth", 0.f, 1.f, 0.f);
    param (l, "macroDarkness", "Darkness", 0.f, 1.f, 0.2f);
    param (l, "macroDirt", "Dirt", 0.f, 1.f, 0.2f);
    param (l, "macroMovement", "Movement", 0.f, 1.f, 0.2f);
    param (l, "macroSpace", "Space", 0.f, 1.f, 0.25f);
    l.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { "engine", 1 }, "Engine",
        juce::StringArray { "Supersaw", "Organ", "Strings", "FM Bell", "Sub 808", "Choir", "Sync Lead",
                            "Wavetable", "Pulse PWM", "Bright EP", "Haunt FM", "Ring Mod", "Noise Pad",
                            "FM Shimmer", "Comb Bow", "Tri Pad", "Brass", "Soft PWM", "Dig Wave", "Bowed",
                            "Swell Bed", "Pluck Keys", "Dist 808", "Impact" },
        0));
    l.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { "arp", 1 }, "Arpeggiator", false));
    l.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { "chord", 1 }, "Chord",
        juce::StringArray { "Off", "Minor", "Minor 7", "Diminished", "Sus 2", "Minor 9" }, 0));
    l.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { "filterType", 1 }, "Filter",
        juce::StringArray { "Lowpass", "Highpass", "Bandpass", "Ladder", "Comb", "Formant" }, 0));
    l.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { "bankMode", 1 }, "Sound bank",
        juce::StringArray { "Church Keys", "Dark Brass", "Trap Orchestra" }, 0));
    return l;
}

TrapPatch TrapLordSynthAudioProcessor::buildPatch() const
{
    const float dark = get (state, "macroDarkness");
    const float dirt = get (state, "macroDirt");
    const float move = get (state, "macroMovement");

    TrapPatch p;
    p.attack = get (state, "attack");
    p.decay = get (state, "decay");
    p.sustain = get (state, "sustain");
    p.release = get (state, "release");
    p.cutoff = get (state, "cutoff") * juce::jmap (dark, 1.f, 0.42f);
    p.gain = get (state, "gain");
    p.drive = get (state, "drive") + dirt * 2.8f;
    p.detune = get (state, "detune") + move * 6.f;
    p.resonance = get (state, "resonance") + dark * 0.12f;
    p.pitchEnvSemi = get (state, "pitchEnvSemi");
    p.pitchEnvDecaySec = juce::jmax (0.001f, get (state, "pitchEnvMs") * 0.001f);
    p.pulseWidth = get (state, "pulseWidth");
    p.portamentoSec = get (state, "portamentoMs") * 0.001f;
    p.vibratoDepth = get (state, "vibratoDepth") + move * 0.18f;
    p.vibratoRate = 4.5f + move * 3.5f;
    p.swellDepth = get (state, "swellDepth");
    p.oscMorph = get (state, "pulseWidth");
    p.unisonVoices = juce::jlimit (1, 7, 1 + int (get (state, "detune") / 5.5f));
    p.noiseMix = juce::jlimit (0.f, 1.f, dirt * 0.55f + dark * 0.15f);
    p.ringModDepth = juce::jlimit (0.f, 1.f, move * 0.55f + dirt * 0.25f);
    p.fmIndex = juce::jlimit (0.2f, 9.f, 1.f + dirt * 6.f + get (state, "pitchEnvSemi") * 0.18f);
    p.fmRatio = juce::jlimit (0.5f, 8.f, 1.2f + get (state, "pulseWidth") * 3.8f);
    p.keyTrack = juce::jlimit (0.55f, 2.2f, 0.9f + dark * 0.45f);
    p.engine = juce::jlimit (0, 23, int (get (state, "engine")));
    p.filterType = juce::jlimit (0, 5, int (get (state, "filterType")));
    return p;
}

void TrapLordSynthAudioProcessor::prepareToPlay (double sr, int block)
{
    currentRate = sr;
    synth.setCurrentPlaybackSampleRate (sr);
    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* v = dynamic_cast<TrapVoice*> (synth.getVoice (i)))
            v->prepare (sr);

    juce::dsp::ProcessSpec spec { sr, static_cast<juce::uint32> (juce::jmax (1, block)), 2 };
    reverb.prepare (spec);
    reverb.reset();
    delay.prepare (spec);
    delay.reset();
    delay.setDelay (juce::jmin (96000.f, float (sr * .375)));
    chorus.prepare (spec);
    chorus.reset();
    chorus.setCentreDelay (7.f);
    chorus.setFeedback (0.08f);

    hpStateL = hpStateR = hpPrevInL = hpPrevInR = 0.f;

    arpPhase = 0.;
    rotaryPhase = 0.;
    arpStep = 0;
    held.fill (false);
}

bool TrapLordSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& l) const
{
    return l.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

double TrapLordSynthAudioProcessor::arpPeriodSeconds() const
{
    const double bpm = juce::jmax (20., hostBpm);
    const double beatsPerStep = 4. / juce::jmax (1.f, get (state, "arpRate"));
    return 60. / bpm * beatsPerStep;
}

void TrapLordSynthAudioProcessor::generateArp (juce::MidiBuffer& midi, int samples)
{
    if (offlineActive)
        return;

    juce::MidiBuffer generated;
    const bool enabled = get (state, "arp") > .5f;
    const int chord = juce::jlimit (0, 5, int (get (state, "chord")));
    const int intervals[][4] = { { 0, -1, -1, -1 }, { 0, 3, 7, -1 },   { 0, 3, 7, 10 },
                                 { 0, 3, 6, -1 },  { 0, 2, 7, -1 },   { 0, 3, 7, 14 } };

    auto addChord = [&] (int note, bool on, int offset)
    {
        for (int interval : intervals[chord])
            if (interval >= 0 && note + interval < 128)
                generated.addEvent (
                    on ? juce::MidiMessage::noteOn (1, note + interval, (juce::uint8) 100)
                       : juce::MidiMessage::noteOff (1, note + interval),
                    offset);
    };

    for (const auto metadata : midi)
    {
        const auto m = metadata.getMessage();
        const int pos = metadata.samplePosition;
        if (m.isNoteOn() || m.isNoteOff())
        {
            const int n = m.getNoteNumber();
            held[size_t (n)] = m.isNoteOn();
            if (! enabled)
                addChord (n, m.isNoteOn(), pos);
        }
        else
        {
            generated.addEvent (m, pos);
        }
    }

    if (enabled)
    {
        const double periodSamples = currentRate * arpPeriodSeconds();
        const double gateEnd = periodSamples * (1. - get (state, "arpGate"));

        for (int pos = 0; pos < samples; ++pos)
        {
            if (arpPhase <= 0.)
            {
                generated.addEvent (juce::MidiMessage::allNotesOff (1), pos);
                std::array<int, 128> notes {};
                int count = 0;
                for (int n = 0; n < 128; ++n)
                    if (held[size_t (n)])
                        notes[size_t (count++)] = n;

                if (count > 0)
                    addChord (notes[size_t (arpStep++ % count)], true, pos);

                arpPhase += periodSamples;
            }

            arpPhase -= 1.;

            if (arpPhase > 0. && arpPhase <= gateEnd && arpPhase + 1. > gateEnd)
                generated.addEvent (juce::MidiMessage::allNotesOff (1), pos);
        }
    }

    midi.swapWith (generated);
}

void TrapLordSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;

    if (currentRate <= 0.)
        return;

    buffer.clear();

    if (auto* head = getPlayHead())
        if (auto pos = head->getPosition())
            if (auto bpm = pos->getBpm())
                if (std::isfinite (*bpm) && *bpm > 0.)
                    hostBpm = *bpm;

    const auto bankSnapshot = bank;
    const TrapPatch p = buildPatch();

    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* v = dynamic_cast<TrapVoice*> (synth.getVoice (i)))
        {
            v->setPatch (p);
            v->setSamples (bankSnapshot);
        }

    generateArp (midi, buffer.getNumSamples());
    synth.renderNextBlock (buffer, midi, 0, buffer.getNumSamples());

    const float move = get (state, "macroMovement");
    const float space = get (state, "macroSpace");
    const float dark = get (state, "macroDarkness");
    const float dirt = get (state, "macroDirt");

    chorus.setRate (0.45f + move * 1.2f);
    chorus.setDepth (move * 0.55f);
    chorus.setMix (move * 0.35f);
    {
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> chorusCtx (block);
        chorus.process (chorusCtx);
    }

    if (move > 0.12f)
    {
        static float apL[6]{}, apR[6]{};
        const float rate = 0.002f + move * 0.012f;
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            for (int ch = 0; ch < 2; ++ch)
            {
                float* d = buffer.getWritePointer (ch);
                float* ap = ch == 0 ? apL : apR;
                float x = d[i];
                for (int s = 0; s < 4; ++s)
                {
                    const float coeff = rate * (1.f + s * 0.22f);
                    const float delayed = ap[s];
                    const float y = -coeff * x + delayed + coeff * x;
                    ap[s] = x;
                    x = y;
                }
                d[i] = juce::jlimit (-1.f, 1.f, x * (1.f - move * 0.25f) + d[i] * (0.75f + move * 0.25f));
            }
        }
    }

    if (dirt > 0.62f)
    {
        const int bits = juce::jmax (6, 14 - int (dirt * 10.f));
        const float crush = std::pow (2.f, float (bits));
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* d = buffer.getWritePointer (ch);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
                d[i] = std::round (d[i] * crush) / crush;
        }
    }

    const int engine = p.engine;
    const float rotaryDepth = juce::jlimit (0.f, 1.f, move * 0.7f + (engine == 1 ? 0.35f : 0.f));
    if (rotaryDepth > 0.01f)
        applyRotary (buffer, rotaryDepth);

    const float wet = juce::jlimit (0.f, 0.55f, get (state, "delay") + space * 0.22f);
    constexpr float kDelayFeedback = 0.18f;
    if (wet > 0.001f)
    {
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            const float l = buffer.getSample (0, i);
            const float r = buffer.getSample (1, i);
            const float dl = delay.popSample (0);
            const float dr = delay.popSample (1);
            delay.pushSample (0, juce::jlimit (-1.5f, 1.5f, l + dl * kDelayFeedback));
            delay.pushSample (1, juce::jlimit (-1.5f, 1.5f, r + dr * kDelayFeedback));
            buffer.setSample (0, i, juce::jlimit (-1.f, 1.f, l + dl * wet));
            buffer.setSample (1, i, juce::jlimit (-1.f, 1.f, r + dr * wet));
        }
    }

    juce::dsp::Reverb::Parameters rp;
    rp.roomSize = juce::jlimit (0.25f, 0.92f, 0.55f + space * 0.35f);
    rp.damping = juce::jlimit (0.2f, 0.95f, 0.45f + dark * 0.4f);
    rp.wetLevel = juce::jlimit (0.f, 1.f, get (state, "reverb") * .45f + space * 0.4f);
    rp.dryLevel = juce::jlimit (0.f, 1.f, 1.f - rp.wetLevel);
    rp.freezeMode = space > 0.65f ? 1.f : 0.f;
    rp.width = 1.f;
    reverb.setParameters (rp);

    juce::dsp::AudioBlock<float> audio (buffer);
    juce::dsp::ProcessContextReplacing<float> context (audio);
    reverb.process (context);

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* d = buffer.getWritePointer (ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float x = std::tanh (d[i]);
            d[i] = std::isfinite (x) ? x : 0.f;
        }
    }

    const float hpCoeff = currentRate > 0.
                            ? std::exp (-juce::MathConstants<float>::twoPi * 20.f / float (currentRate))
                            : 0.f;
    if (hpCoeff > 0.f && buffer.getNumChannels() >= 2)
    {
        auto* l = buffer.getWritePointer (0);
        auto* r = buffer.getWritePointer (1);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            const float inL = l[i];
            const float inR = r[i];
            hpStateL = hpCoeff * (hpStateL + inL - hpPrevInL);
            hpStateR = hpCoeff * (hpStateR + inR - hpPrevInR);
            hpPrevInL = inL;
            hpPrevInR = inR;
            l[i] = std::isfinite (hpStateL) ? hpStateL : 0.f;
            r[i] = std::isfinite (hpStateR) ? hpStateR : 0.f;
        }
    }
}

const juce::StringArray& TrapLordSynthAudioProcessor::factoryNames()
{
    static juce::StringArray names;
    if (names.isEmpty())
        for (int i = 0; i < PresetLibrary::instance().count(); ++i)
            names.add (PresetLibrary::instance().get (i).name);
    return names;
}

int TrapLordSynthAudioProcessor::factoryPresetCount()
{
    return PresetLibrary::instance().count();
}

int TrapLordSynthAudioProcessor::presetBank (int index)
{
    return PresetLibrary::instance().get (index).bank;
}

juce::String TrapLordSynthAudioProcessor::presetCategory (int index)
{
    return PresetLibrary::instance().get (index).category;
}

void TrapLordSynthAudioProcessor::loadFactoryPreset (int idx)
{
    PresetLibrary::instance().applyToState (state, idx);
    currentPresetIndex = idx;
}

void TrapLordSynthAudioProcessor::loadDiagnosticPatch (int kind)
{
    auto set = [&] (const char* id, float val)
    {
        if (auto* p = state.getParameter (id))
            p->setValueNotifyingHost (p->convertTo0to1 (val));
    };

    set ("attack", 0.02f);
    set ("decay", 0.25f);
    set ("sustain", 0.65f);
    set ("release", 0.35f);
    set ("cutoff", 12000.f);
    set ("gain", 0.28f);
    set ("drive", 1.f);
    set ("detune", kind == 0 ? 8.f : 0.f);
    set ("reverb", 0.f);
    set ("delay", 0.f);
    set ("chord", 0);
    set ("resonance", 0.55f);
    set ("macroDarkness", 0.f);
    set ("macroDirt", 0.f);
    set ("macroMovement", 0.f);
    set ("macroSpace", 0.f);
    set ("swellDepth", 0.f);
    set ("engine", float (kind == 1 ? 4 : 6)); // 4 ≈ sine-weighted bass, 6 = lead saw
    set ("arp", 0.f);
    currentPresetIndex = -1;
}

void TrapLordSynthAudioProcessor::prepareOfflineRender (double sampleRate, bool preservePatchParams)
{
    offlineSavedRate = currentRate;
    offlineActive = true;
    setNonRealtime (true);
    hostBpm = 140.;
    prepareToPlay (sampleRate, 512);
    synth.allNotesOff (0, true);
    if (! preservePatchParams)
    {
        if (auto* arp = state.getParameter ("arp"))
            arp->setValueNotifyingHost (0.f);
        if (auto* chord = state.getParameter ("chord"))
            chord->setValueNotifyingHost (0.f);
    }
}

void TrapLordSynthAudioProcessor::finishOfflineRender()
{
    if (! offlineActive)
        return;
    if (offlineSavedRate > 0.)
        prepareToPlay (offlineSavedRate, 512);
    setNonRealtime (false);
    offlineActive = false;
}

bool TrapLordSynthAudioProcessor::renderOfflineTimeline (
    int presetIndex,
    const std::vector<std::pair<int, juce::MidiMessage>>& timeline,
    double sampleRate,
    double durationSeconds,
    double bpm,
    juce::AudioBuffer<float>& buffer)
{
    if (presetIndex >= 0)
    {
        if (presetIndex >= PresetLibrary::instance().count())
            return false;
        loadFactoryPreset (presetIndex);
    }
    hostBpm = bpm;

    const int totalSamples = juce::jmax (1, int (std::round (durationSeconds * sampleRate)));
    buffer.setSize (2, totalSamples, false, false, true);
    buffer.clear();

    prepareOfflineRender (sampleRate, true);

    size_t eventIdx = 0;
    const int block = 512;

    for (int pos = 0; pos < totalSamples; pos += block)
    {
        const int len = juce::jmin (block, totalSamples - pos);
        juce::AudioBuffer<float> part (2, len);
        part.clear();
        juce::MidiBuffer blockMidi;

        while (eventIdx < timeline.size() && timeline[eventIdx].first < pos + len)
        {
            const int offset = juce::jlimit (0, len - 1, timeline[eventIdx].first - pos);
            blockMidi.addEvent (timeline[eventIdx].second, offset);
            ++eventIdx;
        }

        processBlock (part, blockMidi);
        for (int ch = 0; ch < 2; ++ch)
            buffer.copyFrom (ch, pos, part, ch, 0, len);
    }

    float peak = 0.f;
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        peak = juce::jmax (peak, buffer.getMagnitude (ch, 0, buffer.getNumSamples()));
    finishOfflineRender();
    return true;
}

bool TrapLordSynthAudioProcessor::renderNoteOffline (int midiNote,
                                                      int velocity,
                                                      juce::AudioBuffer<float>& buffer,
                                                      double sampleRate,
                                                      float noteHoldSeconds)
{
    buffer.clear();
    const int total = buffer.getNumSamples();
    const float holdSec = noteHoldSeconds > 0.f ? noteHoldSeconds : 1.4f;
    const int noteOffSample = juce::jmin (total - 1, juce::jmax (1, int (std::round (holdSec * sampleRate))));
    prepareOfflineRender (sampleRate);

    for (int pos = 0; pos < total; pos += 512)
    {
        const int len = juce::jmin (512, total - pos);
        juce::AudioBuffer<float> part (2, len);
        part.clear();
        juce::MidiBuffer blockMidi;
        if (pos == 0)
            blockMidi.addEvent (juce::MidiMessage::noteOn (1, midiNote, (juce::uint8) velocity), 0);
        if (pos <= noteOffSample && pos + len > noteOffSample)
            blockMidi.addEvent (juce::MidiMessage::noteOff (1, midiNote), noteOffSample - pos);
        processBlock (part, blockMidi);
        for (int ch = 0; ch < 2; ++ch)
            buffer.copyFrom (ch, pos, part, ch, 0, len);
    }

    finishOfflineRender();
    return true;
}

bool TrapLordSynthAudioProcessor::exportMpcKeygroupOffline (const juce::File& parentFolder,
                                                             const KeygroupExportOptions& options,
                                                             std::atomic<bool>& abortFlag,
                                                             const std::function<void (float, const juce::String&)>& onProgress)
{
    if (! parentFolder.isDirectory() && ! parentFolder.createDirectory().wasOk())
        return false;

    juce::File exportRoot = parentFolder;
    if (options.useExpansionsLayout)
    {
        exportRoot = parentFolder.getChildFile ("Expansions")
                        .getChildFile (MpcXpmWriter::makeSafeName (options.expansionsPackName, 24));
        if (! exportRoot.createDirectory().wasOk())
            return false;
    }

    const juce::String programName = options.folderPrefix
                                     + MpcXpmWriter::makeSafeName (options.programName, 28);
    const auto programFolder = exportRoot.getChildFile (programName);
    if (programFolder.exists())
        programFolder.deleteRecursively();
    if (! programFolder.createDirectory().wasOk())
        return false;

    const double sr = 44100.;
    const int layers = juce::jlimit (1, 4, options.velocityLayers);
    const int velSplits[][4][2] = {
        { { 1, 127 } },
        { { 1, 70 }, { 71, 127 } },
        { { 1, 45 }, { 46, 90 }, { 91, 127 } },
        { { 1, 40 }, { 41, 75 }, { 76, 105 }, { 106, 127 } }
    };

    int keyCount = 0;
    for (int n = options.lowNote; n <= options.highNote; n += juce::jmax (1, options.semitoneStep))
        ++keyCount;

    const auto& presetRec = PresetLibrary::instance().get (currentPresetIndex);
    auto categoryTiming = [] (const juce::String& cat) -> std::tuple<float, float, bool>
    {
        if (cat == "Pads" || cat == "Swells" || cat == "Strings" || cat == "Choir")
            return { 5.2f, 1.25f, true };
        if (cat == "Synth Keys" || cat == "Church Keys/EP" || cat == "Organ" || cat == "Bells" || cat == "Leads")
            return { 2.6f, 0.55f, false };
        return { 1.15f, 0.35f, false };
    };

    float bodySeconds = options.bodySeconds;
    float tailSeconds = options.releaseTailSeconds;
    bool sustainLoop = false;
    if (options.useCategoryTiming)
    {
        const auto [b, t, loop] = categoryTiming (presetRec.category);
        bodySeconds = b;
        tailSeconds = t;
        sustainLoop = loop;
    }

    const TrapPatch patchEnvelope = buildPatch();

    const int bytesPerSample = options.use24Bit ? 3 : 2;
    const int channels = 2;
    const int64 totalWavBytesIfUnbounded =
        int64 (keyCount) * layers * channels * bytesPerSample
        * int64 ((bodySeconds + tailSeconds) * sr);
    const int64 maxBytes = int64 (options.maxProgramMegabytes * 1024.0 * 1024.0);

    if (totalWavBytesIfUnbounded > maxBytes && keyCount > 0 && layers > 0)
    {
        const double maxSeconds =
            double (maxBytes) / (double (keyCount * layers * channels * bytesPerSample) * sr);
        tailSeconds = juce::jmin (tailSeconds, 0.25f);
        bodySeconds = juce::jmax (0.45f, float (maxSeconds - tailSeconds));
    }

    const int totalSamples = juce::jmax (1, int ((bodySeconds + tailSeconds) * sr));
    std::vector<MpcKeygroupZone> keygroups;
    keygroups.reserve (size_t (keyCount));

    const int totalOps = juce::jmax (1, keyCount * layers);
    int op = 0;

    const float ampRelease = juce::jlimit (0.05f, 8.f, options.ampReleaseSeconds);

    for (int note = options.lowNote; note <= options.highNote; note += juce::jmax (1, options.semitoneStep))
    {
        if (abortFlag.load())
            return false;

        MpcKeygroupZone kg;
        kg.lowNote = note;
        kg.highNote = note;
        kg.rootNote = note;
        kg.volumeAttack = patchEnvelope.attack;
        kg.volumeDecay = patchEnvelope.decay;
        kg.volumeSustain = patchEnvelope.sustain;
        kg.volumeRelease = juce::jlimit (0.05f, 8.f, patchEnvelope.release);

        for (int layer = 0; layer < layers; ++layer)
        {
            if (abortFlag.load())
                return false;

            const int vel = layer == 0 ? 58 : (layer == 1 ? 104 : (layer == 2 ? 72 : 118));
            juce::AudioBuffer<float> audio (2, totalSamples);
            audio.clear();
            if (! renderNoteOffline (note, vel, audio, sr, bodySeconds))
                return false;

            float peak = 0.f;
            for (int ch = 0; ch < audio.getNumChannels(); ++ch)
                peak = juce::jmax (peak, audio.getMagnitude (ch, 0, audio.getNumSamples()));
            if (peak < 1e-5f)
                continue;
            if (peak > 0.99f)
                audio.applyGain (0.95f / peak);

            const auto wavName = MpcXpmWriter::makeSampleFilename (note, layer);
            const auto wavFile = programFolder.getChildFile (wavName);
            const int bits = options.use24Bit ? 24 : 16;
            if (! writeKeygroupWav (wavFile, audio, sr, bits))
                return false;

            MpcSampleLayer sampleLayer;
            sampleLayer.rootNote = note;
            sampleLayer.velLow = velSplits[layers - 1][layer][0];
            sampleLayer.velHigh = velSplits[layers - 1][layer][1];
            sampleLayer.sampleName = wavName;
            sampleLayer.sampleFrames = audio.getNumSamples();
            if (sustainLoop && sampleLayer.sampleFrames > 8192)
            {
                const int holdSamples = juce::jmin (sampleLayer.sampleFrames - 1,
                                                    int (std::round (bodySeconds * sr)));
                const int loopStart = juce::jmin (holdSamples - 512,
                                                int (std::round ((patchEnvelope.attack + patchEnvelope.decay) * sr)) + 512);
                const int loopEnd = juce::jmax (loopStart + 4096, holdSamples - int (sr * 0.08));
                if (loopEnd < sampleLayer.sampleFrames && loopEnd > loopStart + 2048)
                {
                    sampleLayer.loopStart = loopStart;
                    sampleLayer.loopEnd = juce::jmin (loopEnd, sampleLayer.sampleFrames - 1);
                }
            }
            kg.layers.push_back (sampleLayer);

            ++op;
            if (onProgress)
                onProgress (float (op) / float (totalOps), "Rendering " + wavName);
        }

        if (! kg.layers.empty())
            keygroups.push_back (std::move (kg));
    }

    if (keygroups.empty() || abortFlag.load())
        return false;

    if (! MpcXpmWriter::writeKeygroupProgram (programFolder, programName, keygroups))
        return false;

    if (onProgress)
        onProgress (1.f, "Wrote " + programName + ".xpm");

    return true;
}

void TrapLordSynthAudioProcessor::applyRotary (juce::AudioBuffer<float>& buffer, float depth)
{
    auto* l = buffer.getWritePointer (0);
    auto* r = buffer.getWritePointer (1);
    const int n = buffer.getNumSamples();
    const double inc = 6.2 / juce::jmax (1., currentRate);

    for (int i = 0; i < n; ++i)
    {
        rotaryPhase += inc;
        if (rotaryPhase > 1.)
            rotaryPhase -= 1.;
        const float mod = std::sin (float (rotaryPhase * juce::MathConstants<double>::twoPi));
        const float trem = 1.f - depth * 0.28f * (1.f - mod);
        const float pan = depth * 0.35f * mod;
        const float li = l[i] * trem * (1.f - pan);
        const float ri = r[i] * trem * (1.f + pan);
        l[i] = std::isfinite (li) ? li : 0.f;
        r[i] = std::isfinite (ri) ? ri : 0.f;
    }
}

bool TrapLordSynthAudioProcessor::loadSampleMap (const juce::File& jsonFile)
{
    auto parsed = juce::JSON::parse (jsonFile);
    if (! parsed.isArray())
        return false;

    auto next = std::make_shared<std::vector<TrapSamplePtr>>();
    juce::AudioFormatManager formats;
    formats.registerBasicFormats();

    for (const auto& entry : *parsed.getArray())
    {
        const auto filename = entry.getProperty ("file", juce::var()).toString();
        auto file = jsonFile.getParentDirectory().getChildFile (filename);
        std::unique_ptr<juce::AudioFormatReader> reader (formats.createReaderFor (file));
        if (! reader || reader->lengthInSamples < 2 || reader->lengthInSamples > 44100LL * 60 * 5)
            continue;

        auto sample = std::make_shared<TrapSample>();
        sample->root = int (entry.getProperty ("root", 60));
        sample->low = int (entry.getProperty ("low", 0));
        sample->high = int (entry.getProperty ("high", 127));
        sample->velLow = int (entry.getProperty ("velLow", 0));
        sample->velHigh = int (entry.getProperty ("velHigh", 127));
        sample->sourceRate = reader->sampleRate;
        sample->audio.setSize (1, int (reader->lengthInSamples));

        juce::AudioBuffer<float> tmp (int (reader->numChannels), int (reader->lengthInSamples));
        reader->read (&tmp, 0, tmp.getNumSamples(), 0, true, true);
        for (int i = 0; i < tmp.getNumSamples(); ++i)
        {
            float v = 0.f;
            for (int ch = 0; ch < tmp.getNumChannels(); ++ch)
                v += tmp.getSample (ch, i);
            sample->audio.setSample (0, i, v / float (tmp.getNumChannels()));
        }
        next->push_back (sample);
    }

    if (next->empty())
        return false;

    bank = next;
    return true;
}

bool TrapLordSynthAudioProcessor::exportKeygroup (const juce::File& folder, int root)
{
    if (folder.createDirectory().failed())
        return false;

    juce::Array<juce::var> mappings;
    const int savedEngine = int (get (state, "engine"));
    auto* engineParam = state.getParameter ("engine");
    const double sr = 44100.;
    const double savedRate = currentRate;

    suspendProcessing (true);
    prepareToPlay (sr, 512);

    for (int note = 36; note <= 96; note += 3)
    {
        juce::AudioBuffer<float> audio (2, 88200);
        audio.clear();
        engineParam->setValueNotifyingHost (engineParam->convertTo0to1 (float (savedEngine)));

        for (int pos = 0; pos < audio.getNumSamples(); pos += 512)
        {
            const int len = juce::jmin (512, audio.getNumSamples() - pos);
            juce::AudioBuffer<float> part (2, len);
            juce::MidiBuffer midi;
            if (pos == 0)
                midi.addEvent (juce::MidiMessage::noteOn (1, note, (juce::uint8) 110), 0);
            if (pos <= 22050 && pos + len > 22050)
                midi.addEvent (juce::MidiMessage::noteOff (1, note), 22050 - pos);
            processBlock (part, midi);
            for (int ch = 0; ch < 2; ++ch)
                audio.copyFrom (ch, pos, part, ch, 0, len);
        }

        const auto filename = juce::String ("TL_") + juce::String (note) + ".wav";
        if (! writeKeygroupWav (folder.getChildFile (filename), audio, sr, 16))
        {
            suspendProcessing (false);
            return false;
        }

        auto item = std::make_unique<juce::DynamicObject>();
        item->setProperty ("file", filename);
        item->setProperty ("root", note);
        item->setProperty ("low", juce::jmax (0, note - 1));
        item->setProperty ("high", juce::jmin (127, note + 1));
        mappings.add (juce::var (item.release()));
    }

    folder.getChildFile ("keygroup.json").replaceWithText (juce::JSON::toString (juce::var (mappings), true));
    prepareToPlay (savedRate, 512);
    suspendProcessing (false);
    juce::ignoreUnused (root);
    return true;
}

void TrapLordSynthAudioProcessor::getStateInformation (juce::MemoryBlock& out)
{
    if (auto xml = state.copyState().createXml())
        copyXmlToBinary (*xml, out);
}

void TrapLordSynthAudioProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size))
        if (xml->hasTagName (state.state.getType()))
            state.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessorEditor* TrapLordSynthAudioProcessor::createEditor()
{
    return new TrapLordSynthAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TrapLordSynthAudioProcessor();
}
