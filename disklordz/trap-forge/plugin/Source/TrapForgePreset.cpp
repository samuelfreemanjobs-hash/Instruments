#include "TrapForgePreset.h"

namespace trapforge
{

DrumCategory categoryFromString (const juce::String& cat)
{
    const auto u = cat.toUpperCase();
    if (u == "808") return DrumCategory::sub808;
    if (u == "SNARE") return DrumCategory::snare;
    if (u == "CLAP") return DrumCategory::clap;
    if (u == "HIHAT") return DrumCategory::hihat;
    if (u == "PERC") return DrumCategory::perc;
    return DrumCategory::kick;
}

static float getFloat (const juce::var& obj, const char* key, float fallback)
{
    if (auto* o = obj.getDynamicObject())
        if (o->hasProperty (key))
            return static_cast<float> (static_cast<double> (o->getProperty (key)));
    return fallback;
}

TrapForgePreset presetFromJson (const juce::var& json)
{
    TrapForgePreset p;
    if (auto* root = json.getDynamicObject())
    {
        p.name = root->getProperty ("name").toString();
        p.category = categoryFromString (root->getProperty ("cat").toString());
        p.rootHz = getFloat (json, "rootHz", p.rootHz);
        p.pitchMod = getFloat (json, "pitchMod", p.pitchMod);
        p.pitchDecay = getFloat (json, "pitchDecay", p.pitchDecay);
        p.glideMs = getFloat (json, "glideMs", p.glideMs);
        p.glideSemi = getFloat (json, "glideSemi", getFloat (json, "glideInterval", p.glideSemi));
        p.glideExponent = getFloat (json, "glideExponent", p.glideExponent);
        if (auto amp = root->getProperty ("amp"); amp.isObject())
        {
            p.ampAttack = getFloat (amp, "attack", p.ampAttack);
            p.ampDecay = getFloat (amp, "decay", p.ampDecay);
            p.ampSustain = getFloat (amp, "sustain", p.ampSustain);
            p.ampRelease = getFloat (amp, "release", p.ampRelease);
        }
        if (auto tx = root->getProperty ("tx"); tx.isObject())
        {
            p.drive = getFloat (tx, "drive", p.drive) / 100.0f;
            p.outputCeilingDb = getFloat (tx, "ceiling", p.outputCeilingDb);
        }
    }
    return p;
}

juce::Array<TrapForgePreset> loadFactoryPresets()
{
    juce::Array<TrapForgePreset> out;
    const juce::String presets[] = {
        R"({"name":"METRO_THUMP_KICK","cat":"KICK","rootHz":52,"pitchMod":48,"pitchDecay":0.035,"amp":{"attack":0.001,"decay":0.28,"sustain":0,"release":0.3},"tx":{"drive":40,"ceiling":-0.1}})",
        R"({"name":"MIKE808_GLIDE","cat":"808","rootHz":47,"glideMs":120,"glideSemi":7,"glideInterval":7,"amp":{"attack":0.003,"decay":0.8,"sustain":0.85,"release":1.2},"tx":{"drive":68,"ceiling":-0.1}})",
        R"({"name":"SOUTHSIDE_SNARE","cat":"SNARE","rootHz":185,"pitchMod":8,"pitchDecay":0.02,"amp":{"attack":0.001,"decay":0.18,"sustain":0.05,"release":0.22},"tx":{"drive":30,"noise":75,"ceiling":-0.2}})",
    };
    for (const auto& s : presets)
    {
        const auto parsed = juce::JSON::parse (s);
        if (! parsed.isVoid())
            out.add (presetFromJson (parsed));
    }
    return out;
}

} // namespace trapforge
