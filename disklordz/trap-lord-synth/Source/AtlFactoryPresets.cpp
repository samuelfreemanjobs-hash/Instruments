#include "AtlFactoryPresets.h"

namespace
{
struct Preset
{
    const char* name;
    AtlFactoryPresets::Bank bank;
    int engine;
    float attack, decay, sustain, release;
    float cutoff, gain, drive, detune;
    float reverb, delay;
    int chord;
    float resonance, pitchEnvSemi, pitchEnvMs, pulseWidth, portamentoMs;
    float vibratoDepth;
    float macroDark, macroDirt, macroMove, macroSpace;
};

static void setParam (juce::AudioProcessorValueTreeState& state, const char* id, float val)
{
    if (auto* p = state.getParameter (id))
        p->setValueNotifyingHost (p->convertTo0to1 (val));
}

// Original ATL Classics recipes — neutral names only.
static constexpr Preset kPresets[] = {
    // Church Keys (6)
    { "Chapel Bright EP", AtlFactoryPresets::churchKeys, 9,
      .012f, .35f, .72f, .45f, 6200.f, .22f, 1.1f, 5.f,
      .32f, .08f, 2, .55f, 0.f, 0.f, .5f, 0.f, .08f,
      .12f, .15f, .45f, .38f },
    { "Bell Choir Keys", AtlFactoryPresets::churchKeys, 10,
      .003f, .22f, .35f, .55f, 8400.f, .2f, 1.f, 4.f,
      .4f, .05f, 5, .5f, 0.f, 0.f, .5f, 0.f, .06f,
      .08f, .08f, .5f, .45f },
    { "Warm Tine Gospel", AtlFactoryPresets::churchKeys, 9,
      .018f, .4f, .78f, .5f, 4800.f, .24f, 1.2f, 7.f,
      .28f, .12f, 2, .6f, 0.f, 0.f, .55f, 0.f, .1f,
      .1f, .2f, .42f, .35f },
    { "Minor Seven Pad", AtlFactoryPresets::churchKeys, 9,
      .25f, .55f, .82f, .9f, 3500.f, .18f, 1.f, 6.f,
      .38f, .06f, 2, .65f, 0.f, 0.f, .5f, 0.f, .07f,
      .18f, .05f, .35f, .5f },
    { "Minor Nine Stack", AtlFactoryPresets::churchKeys, 10,
      .008f, .3f, .7f, .6f, 7200.f, .21f, 1.05f, 5.f,
      .36f, .07f, 5, .52f, 0.f, 0.f, .48f, 0.f, .09f,
      .1f, .12f, .48f, .42f },
    { "Hymnal Rotary Organ", AtlFactoryPresets::churchKeys, 1,
      .04f, .2f, .9f, .7f, 5500.f, .2f, 1.15f, 3.f,
      .25f, .04f, 0, .58f, 0.f, 0.f, .5f, 0.f, .22f,
      .08f, .1f, .65f, .3f },

    // Dark Brass (6)
    { "Dirty Trap Brass", AtlFactoryPresets::darkBrass, 0,
      .005f, .26f, .65f, .18f, 2400.f, .26f, 2.3f, 9.f,
      .22f, .04f, 0, .85f, 3.f, 45.f, .5f, 0.f, 0.f,
      .35f, .55f, .15f, .22f },
    { "Evil Analog Lead", AtlFactoryPresets::darkBrass, 8,
      .008f, .18f, .7f, .28f, 1800.f, .24f, 1.9f, 4.f,
      .12f, .08f, 0, 1.1f, 0.f, 0.f, .14f, 85.f, .05f,
      .55f, .45f, .25f, .12f },
    { "Shadow Brass Hit", AtlFactoryPresets::darkBrass, 0,
      .003f, .32f, .55f, .22f, 2800.f, .28f, 2.6f, 11.f,
      .18f, .05f, 0, .9f, 2.f, 40.f, .5f, 0.f, 0.f,
      .45f, .65f, .1f, .18f },
    { "Aggressive Stab Lead", AtlFactoryPresets::darkBrass, 6,
      .002f, .2f, .6f, .16f, 3200.f, .27f, 2.8f, 8.f,
      .14f, .1f, 0, .75f, 1.f, 35.f, .42f, 25.f, .04f,
      .4f, .7f, .22f, .15f },
    { "Ominous Horn Layer", AtlFactoryPresets::darkBrass, 0,
      .01f, .45f, .75f, .35f, 2100.f, .22f, 1.7f, 13.f,
      .26f, .06f, 0, .8f, 0.f, 0.f, .5f, 0.f, 0.f,
      .5f, .35f, .18f, .28f },
    { "Dark Attack Brass", AtlFactoryPresets::darkBrass, 0,
      .004f, .28f, .62f, .2f, 2600.f, .29f, 2.4f, 10.f,
      .2f, .06f, 0, .88f, 2.5f, 42.f, .5f, 0.f, 0.f,
      .38f, .58f, .12f, .2f },

    // Trap Orchestra (6)
    { "Dark Orchestral Strings", AtlFactoryPresets::trapOrchestra, 2,
      .9f, .7f, .85f, 1.1f, 4200.f, .19f, 1.05f, 14.f,
      .3f, .05f, 0, .62f, 0.f, 0.f, .5f, 0.f, 0.f,
      .42f, .08f, .55f, .48f },
    { "Haunted Trap Bells", AtlFactoryPresets::trapOrchestra, 10,
      .002f, .15f, .2f, 1.8f, 9500.f, .18f, 1.1f, 2.f,
      .58f, .02f, 0, .45f, 0.f, 0.f, .5f, 0.f, .04f,
      .48f, .12f, .2f, .62f },
    { "Southern Church Organ", AtlFactoryPresets::trapOrchestra, 1,
      .06f, .25f, .88f, .85f, 4800.f, .21f, 1.25f, 2.f,
      .32f, .04f, 0, .6f, 0.f, 0.f, .5f, 0.f, .28f,
      .22f, .15f, .72f, .35f },
    { "Cinematic Brass Hit", AtlFactoryPresets::trapOrchestra, 0,
      .008f, .5f, .7f, .65f, 3000.f, .23f, 1.6f, 12.f,
      .45f, .08f, 0, .7f, 1.5f, 50.f, .5f, 0.f, 0.f,
      .3f, .4f, .28f, .55f },
    { "Dramatic String Rise", AtlFactoryPresets::trapOrchestra, 2,
      1.2f, .8f, .9f, 1.4f, 3800.f, .17f, 1.f, 16.f,
      .42f, .06f, 0, .55f, 0.f, 0.f, .5f, 0.f, 0.f,
      .35f, .05f, .62f, .52f },
    { "Layered Orchestra Pulse", AtlFactoryPresets::trapOrchestra, 0,
      .15f, .55f, .78f, .8f, 3600.f, .2f, 1.35f, 11.f,
      .48f, .1f, 0, .68f, 0.f, 0.f, .5f, 0.f, 0.f,
      .28f, .22f, .38f, .58f },
};
} // namespace

const juce::StringArray& AtlFactoryPresets::names()
{
    static juce::StringArray list;
    if (list.isEmpty())
        for (const auto& p : kPresets)
            list.add (p.name);
    return list;
}

AtlFactoryPresets::Bank AtlFactoryPresets::bankForIndex (int index)
{
    if (index < 0 || index >= kPresetCount)
        return churchKeys;
    return kPresets[size_t (index)].bank;
}

void AtlFactoryPresets::apply (juce::AudioProcessorValueTreeState& state, int index)
{
    if (index < 0 || index >= kPresetCount)
        return;

    const auto& p = kPresets[size_t (index)];
    setParam (state, "engine", float (p.engine));
    setParam (state, "attack", p.attack);
    setParam (state, "decay", p.decay);
    setParam (state, "sustain", p.sustain);
    setParam (state, "release", p.release);
    setParam (state, "cutoff", p.cutoff);
    setParam (state, "gain", p.gain);
    setParam (state, "drive", p.drive);
    setParam (state, "detune", p.detune);
    setParam (state, "reverb", p.reverb);
    setParam (state, "delay", p.delay);
    setParam (state, "chord", float (p.chord));
    setParam (state, "resonance", p.resonance);
    setParam (state, "pitchEnvSemi", p.pitchEnvSemi);
    setParam (state, "pitchEnvMs", p.pitchEnvMs);
    setParam (state, "pulseWidth", p.pulseWidth);
    setParam (state, "portamentoMs", p.portamentoMs);
    setParam (state, "vibratoDepth", p.vibratoDepth);
    setParam (state, "macroDarkness", p.macroDark);
    setParam (state, "macroDirt", p.macroDirt);
    setParam (state, "macroMovement", p.macroMove);
    setParam (state, "macroSpace", p.macroSpace);
    setParam (state, "bankMode", float (int (p.bank)));
}
