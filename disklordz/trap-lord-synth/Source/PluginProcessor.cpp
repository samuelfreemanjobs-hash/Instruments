#include "PluginProcessor.h"
#include "PluginEditor.h"
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
    l.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { "engine", 1 }, "Engine",
        juce::StringArray { "Brass", "Organ", "Strings", "FM Bell", "Bass", "Choir", "Lead", "Multisample" }, 0));
    l.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { "arp", 1 }, "Arpeggiator", false));
    l.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { "chord", 1 }, "Chord",
        juce::StringArray { "Off", "Minor", "Minor 7", "Diminished", "Sus 2", "Minor 9" }, 0));
    return l;
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
    arpPhase = 0.;
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
    TrapPatch p;
    p.attack = get (state, "attack");
    p.decay = get (state, "decay");
    p.sustain = get (state, "sustain");
    p.release = get (state, "release");
    p.cutoff = get (state, "cutoff");
    p.gain = get (state, "gain");
    p.drive = get (state, "drive");
    p.detune = get (state, "detune");
    p.engine = juce::jlimit (0, 7, int (get (state, "engine")));

    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* v = dynamic_cast<TrapVoice*> (synth.getVoice (i)))
        {
            v->setPatch (p);
            v->setSamples (bankSnapshot);
        }

    generateArp (midi, buffer.getNumSamples());
    synth.renderNextBlock (buffer, midi, 0, buffer.getNumSamples());

    const float wet = get (state, "delay");
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
    rp.wetLevel = juce::jlimit (0.f, 1.f, get (state, "reverb") * .4f);
    rp.dryLevel = juce::jlimit (0.f, 1.f, 1.f - rp.wetLevel);
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
    static const juce::StringArray names {
        "Frost Brass", "Stab Stack", "Cathedral Organ", "Street Strings",
        "Crystal Bell", "Sub Commander", "Ghost Choir", "Saw Lead",
        "Cold Pad", "Dirty Glide", "Midnight Organ", "Ice Bell",
        "Menace Strings", "Pulse Lead", "Haunted Bells", "Trap March"
    };
    return names;
}

void TrapLordSynthAudioProcessor::loadFactoryPreset (int idx)
{
    static constexpr int engines[] = { 0, 0, 1, 2, 3, 4, 5, 6, 2, 4, 1, 3, 2, 6, 3, 0 };
    if (idx < 0 || idx >= factoryNames().size())
        return;

    auto set = [this] (const char* id, float val)
    {
        if (auto* p = state.getParameter (id))
            p->setValueNotifyingHost (p->convertTo0to1 (val));
    };

    set ("engine", float (engines[idx]));
    set ("attack", idx == 3 || idx == 8 ? .8f : .01f);
    set ("release", idx == 3 || idx == 8 ? 2.f : .35f);
    set ("cutoff", idx == 5 ? 350.f : idx == 4 ? 10000.f : 4500.f);
    set ("drive", idx == 1 || idx == 9 ? 2.5f : 1.f);
    set ("reverb", idx == 5 ? .05f : .25f);
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
