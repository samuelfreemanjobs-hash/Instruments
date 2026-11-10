#include "PresetLibrary.h"

PresetRecord PresetLibrary::empty {};

PresetLibrary& PresetLibrary::instance()
{
    static PresetLibrary lib;
    return lib;
}

bool PresetLibrary::loadFromBinaryData (const void* data, size_t size)
{
    presets.clear();
    const juce::String text (static_cast<const char*> (data), size);
    auto parsed = juce::JSON::parse (text);
    if (! parsed.isObject() || ! parsed.hasProperty ("presets"))
        return false;

    for (const auto& item : *parsed.getProperty ("presets", {}).getArray())
    {
        PresetRecord rec;
        rec.name = item.getProperty ("name", "").toString();
        rec.category = item.getProperty ("category", "Brass").toString();
        rec.bank = int (item.getProperty ("bank", 0));
        rec.params = item;
        if (rec.name.isNotEmpty())
            presets.push_back (rec);
    }
    return presets.size() >= 1;
}

const PresetRecord& PresetLibrary::get (int index) const
{
    if (index >= 0 && index < int (presets.size()))
        return presets[size_t (index)];
    return empty;
}

void PresetLibrary::applyToState (juce::AudioProcessorValueTreeState& state, int index) const
{
    const auto& rec = get (index);
    if (rec.name.isEmpty())
        return;

    auto set = [&] (const char* id, float val)
    {
        if (auto* p = state.getParameter (id))
            p->setValueNotifyingHost (p->convertTo0to1 (val));
    };

    const auto& o = rec.params;
    auto f = [&] (const char* k, float def) -> float
    {
        return float (o.getProperty (k, def));
    };

    set ("engine", f ("engine", 0));
    set ("attack", f ("attack", 0.01f));
    set ("decay", f ("decay", 0.3f));
    set ("sustain", f ("sustain", 0.7f));
    set ("release", f ("release", 0.4f));
    set ("cutoff", f ("cutoff", 4500.f));
    set ("gain", f ("gain", 0.2f));
    set ("drive", f ("drive", 1.f));
    set ("detune", f ("detune", 7.f));
    set ("reverb", f ("reverb", 0.22f));
    set ("delay", f ("delay", 0.05f));
    set ("chord", f ("chord", 0));
    set ("resonance", f ("resonance", 0.55f));
    set ("pitchEnvSemi", f ("pitchEnvSemi", 0));
    set ("pitchEnvMs", f ("pitchEnvMs", 45));
    set ("pulseWidth", f ("pulseWidth", 0.5f));
    set ("portamentoMs", f ("portamentoMs", 0));
    set ("vibratoDepth", f ("vibratoDepth", 0));
    set ("macroDarkness", f ("macroDarkness", 0.2f));
    set ("macroDirt", f ("macroDirt", 0.2f));
    set ("macroMovement", f ("macroMovement", 0.2f));
    set ("macroSpace", f ("macroSpace", 0.25f));
    set ("bankMode", float (rec.bank));
}

juce::StringArray PresetLibrary::categories() const
{
    juce::StringArray cats;
    for (const auto& p : presets)
        if (! cats.contains (p.category))
            cats.add (p.category);
    cats.sort (true);
    return cats;
}

juce::Array<int> PresetLibrary::indicesForCategory (const juce::String& category) const
{
    juce::Array<int> out;
    for (int i = 0; i < int (presets.size()); ++i)
        if (category.isEmpty() || presets[size_t (i)].category == category)
            out.add (i);
    return out;
}

juce::Array<int> PresetLibrary::search (const juce::String& text, const juce::String& categoryFilter) const
{
    juce::Array<int> out;
    const auto needle = text.trim().toLowerCase();
    for (int i = 0; i < int (presets.size()); ++i)
    {
        const auto& p = presets[size_t (i)];
        if (categoryFilter.isNotEmpty() && p.category != categoryFilter)
            continue;
        if (needle.isEmpty() || p.name.toLowerCase().contains (needle))
            out.add (i);
    }
    return out;
}
