#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PresetLibrary.h"
#include "MpcXpmWriter.h"
#include "BinaryData.h"
#include <cmath>

namespace
{
using Layout = juce::AudioProcessorValueTreeState::ParameterLayout;

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
        juce::StringArray { "Brass", "Organ", "Strings", "FM Bell", "Bass", "Choir", "Lead", "Multisample",
                            "Analog Pulse", "Bright EP", "Haunted FM" },
        0));
    l.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { "arp", 1 }, "Arpeggiator", false));
    l.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { "chord", 1 }, "Chord",
        juce::StringArray { "Off", "Minor", "Minor 7", "Diminished", "Sus 2", "Minor 9" }, 0));
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
    p.engine = juce::jlimit (0, 10, int (get (state, "engine")));
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

    chorus.setRate (0.45f + move * 1.2f);
    chorus.setDepth (move * 0.55f);
    chorus.setMix (move * 0.35f);
    {
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> chorusCtx (block);
        chorus.process (chorusCtx);
    }

    const int engine = p.engine;
    const float rotaryDepth = juce::jlimit (0.f, 1.f, move * 0.7f + (engine == 1 ? 0.35f : 0.f));
    if (rotaryDepth > 0.01f)
        applyRotary (buffer, rotaryDepth);

    const float wet = juce::jlimit (0.f, 0.7f, get (state, "delay") + space * 0.28f);
    if (wet > 0.f)
    {
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            const float l = buffer.getSample (0, i);
            const float r = buffer.getSample (1, i);
            const float dl = delay.popSample (0);
            const float dr = delay.popSample (1);
            delay.pushSample (0, l + dl * .35f);
            delay.pushSample (1, r + dr * .35f);
            buffer.setSample (0, i, l + dl * wet);
            buffer.setSample (1, i, r + dr * wet);
        }
    }
    else
    {
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            delay.popSample (0);
            delay.popSample (1);
            delay.pushSample (0, buffer.getSample (0, i));
            delay.pushSample (1, buffer.getSample (1, i));
        }
    }

    juce::dsp::Reverb::Parameters rp;
    rp.roomSize = .65f;
    rp.damping = .5f;
    rp.wetLevel = juce::jlimit (0.f, 1.f, get (state, "reverb") * .4f + space * 0.35f);
    rp.dryLevel = juce::jlimit (0.f, 1.f, 1.f - rp.wetLevel);
    rp.damping = juce::jlimit (0.2f, 0.95f, 0.45f + dark * 0.4f);
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

void TrapLordSynthAudioProcessor::prepareOfflineRender (double sampleRate)
{
    offlineSavedRate = currentRate;
    offlineActive = true;
    suspendProcessing (true);
    hostBpm = 140.;
    prepareToPlay (sampleRate, 512);
    if (auto* arp = state.getParameter ("arp"))
        arp->setValueNotifyingHost (arp->getDefaultValue());
}

void TrapLordSynthAudioProcessor::finishOfflineRender()
{
    if (! offlineActive)
        return;
    prepareToPlay (offlineSavedRate, 512);
    suspendProcessing (false);
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
    if (presetIndex < 0 || presetIndex >= PresetLibrary::instance().count())
        return false;

    loadFactoryPreset (presetIndex);
    hostBpm = bpm;

    const int totalSamples = juce::jmax (1, int (std::round (durationSeconds * sampleRate)));
    buffer.setSize (2, totalSamples, false, false, true);
    buffer.clear();

    prepareOfflineRender (sampleRate);

    size_t eventIdx = 0;
    const int block = 512;

    for (int pos = 0; pos < totalSamples; pos += block)
    {
        const int len = juce::jmin (block, totalSamples - pos);
        juce::AudioBuffer<float> part (2, len);
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
    if (peak > 1e-5f && peak > 0.99f)
        buffer.applyGain (0.95f / peak);

    finishOfflineRender();
    return true;
}

bool TrapLordSynthAudioProcessor::renderNoteOffline (int midiNote,
                                                      int velocity,
                                                      juce::AudioBuffer<float>& buffer,
                                                      double sampleRate)
{
    const double savedRate = currentRate;
    suspendProcessing (true);
    prepareToPlay (sampleRate, 512);

    buffer.clear();

    const int total = buffer.getNumSamples();
  const int noteOffSample = juce::jmax (0, total - int (sampleRate * 0.05));

    for (int pos = 0; pos < total; pos += 512)
    {
        const int len = juce::jmin (512, total - pos);
        juce::AudioBuffer<float> part (2, len);
        juce::MidiBuffer blockMidi;
        if (pos == 0)
            blockMidi.addEvent (juce::MidiMessage::noteOn (1, midiNote, (juce::uint8) velocity), 0);
        if (pos <= noteOffSample && pos + len > noteOffSample)
            blockMidi.addEvent (juce::MidiMessage::noteOff (1, midiNote), noteOffSample - pos);
        processBlock (part, blockMidi);
        for (int ch = 0; ch < 2; ++ch)
            buffer.copyFrom (ch, pos, part, ch, 0, len);
    }

    prepareToPlay (savedRate, 512);
    suspendProcessing (false);
    return true;
}

bool TrapLordSynthAudioProcessor::exportMpcKeygroupOffline (const juce::File& parentFolder,
                                                             const KeygroupExportOptions& options,
                                                             std::atomic<bool>& abortFlag,
                                                             const std::function<void (float, const juce::String&)>& onProgress)
{
    if (! parentFolder.isDirectory() && ! parentFolder.createDirectory().wasOk())
        return false;

    const juce::String programName = MpcXpmWriter::makeSafeName (options.programName);
    const auto programFolder = parentFolder.getChildFile (programName);
    if (! programFolder.createDirectory().wasOk())
        return false;

    const double sr = 44100.;
    const int totalSamples = int ((options.bodySeconds + options.releaseTailSeconds) * sr);
    std::vector<MpcSampleZone> zones;

    const int layers = juce::jlimit (1, 3, options.velocityLayers);
    const int velSplits[][3][2] = { { { 1, 127 } }, { { 1, 70 }, { 71, 127 } }, { { 1, 50 }, { 51, 90 }, { 91, 127 } } };

    int steps = 0;
    for (int n = options.lowNote; n <= options.highNote; n += juce::jmax (1, options.semitoneStep))
        steps++;
    const int totalOps = juce::jmax (1, steps * layers);
    int op = 0;

    juce::AudioFormatManager fm;
    fm.registerBasicFormats();

    for (int note = options.lowNote; note <= options.highNote; note += juce::jmax (1, options.semitoneStep))
    {
        for (int layer = 0; layer < layers; ++layer)
        {
            if (abortFlag.load())
                return false;

            const int vel = layer == 0 ? 54 : (layer == 1 ? 96 : 118);
            juce::AudioBuffer<float> audio (2, totalSamples);
            audio.clear();
            if (! renderNoteOffline (note, vel, audio, sr))
                return false;

            float peak = 0.f;
            for (int ch = 0; ch < audio.getNumChannels(); ++ch)
                peak = juce::jmax (peak, audio.getMagnitude (ch, 0, audio.getNumSamples()));
            if (peak < 1e-5f)
                continue;
            if (peak > 0.99f)
                audio.applyGain (0.95f / peak);

            const auto wavName = juce::String ("TL_N") + juce::String (note) + "_V"
                               + juce::String (velSplits[layers - 1][layer][0]) + "_"
                               + juce::String (velSplits[layers - 1][layer][1]) + ".WAV";
            const auto wavFile = programFolder.getChildFile (wavName);
            auto stream = wavFile.createOutputStream();
            if (! stream)
                return false;

            juce::WavAudioFormat wav;
            const int bits = options.use24Bit ? 24 : 16;
            auto writer = std::unique_ptr<juce::AudioFormatWriter> (
                wav.createWriterFor (stream.get(), sr, 2, bits, {}, 0));
            if (! writer)
                return false;
            stream.release();
            writer->writeFromAudioSampleBuffer (audio, 0, audio.getNumSamples());

            MpcSampleZone zone;
            zone.rootNote = note;
            zone.lowNote = juce::jmax (options.lowNote, note - 1);
            zone.highNote = juce::jmin (options.highNote, note + 1);
            zone.velLow = velSplits[layers - 1][layer][0];
            zone.velHigh = velSplits[layers - 1][layer][1];
            zone.sampleName = wavName;
            zone.sampleFrames = audio.getNumSamples();
            zones.push_back (zone);

            ++op;
            if (onProgress)
                onProgress (float (op) / float (totalOps), "Rendering " + wavName);
        }
    }

    if (zones.empty() || abortFlag.load())
        return false;

    if (! MpcXpmWriter::writeKeygroupProgram (programFolder, programName, zones))
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
        auto stream = folder.getChildFile (filename).createOutputStream();
        if (! stream)
        {
            suspendProcessing (false);
            return false;
        }

        juce::WavAudioFormat wav;
        auto writer = std::unique_ptr<juce::AudioFormatWriter> (
            wav.createWriterFor (stream.get(), sr, 2, 16, {}, 0));
        if (! writer)
        {
            suspendProcessing (false);
            return false;
        }

        stream.release();
        writer->writeFromAudioSampleBuffer (audio, 0, audio.getNumSamples());

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
