#include "VibeRecipeJson.h"

namespace resonance::audio
{
namespace
{
juce::String targetKindToString(VibeMixTargetKind k)
{
    switch (k)
    {
        case VibeMixTargetKind::Master:
            return "master";
        case VibeMixTargetKind::Drums:
            return "drums";
        case VibeMixTargetKind::Vocals:
            return "vocals";
        case VibeMixTargetKind::Synths:
            return "synths";
        case VibeMixTargetKind::FullMix:
            return "full_mix";
        case VibeMixTargetKind::Channel:
            return "channel";
    }
    return "master";
}

VibeMixTargetKind targetKindFromString(const juce::String& s)
{
    if (s == "drums")
        return VibeMixTargetKind::Drums;
    if (s == "vocals")
        return VibeMixTargetKind::Vocals;
    if (s == "synths")
        return VibeMixTargetKind::Synths;
    if (s == "full_mix")
        return VibeMixTargetKind::FullMix;
    if (s == "channel")
        return VibeMixTargetKind::Channel;
    return VibeMixTargetKind::Master;
}
} // namespace

juce::String VibeRecipeJson::toJson(const VibeMixRecipe& recipe)
{
    juce::DynamicObject::Ptr root = new juce::DynamicObject();
    root->setProperty("schemaVersion", recipe.schemaVersion);
    root->setProperty("title", recipe.title);
    root->setProperty("explanation", recipe.explanation);
    root->setProperty("target", targetKindToString(recipe.target.kind));
    root->setProperty("channelIndex", recipe.target.channelIndex);

    juce::Array<juce::var> conflicts;
    for (const auto& c : recipe.conflicts)
        conflicts.add(c);
    root->setProperty("conflicts", conflicts);

    juce::Array<juce::var> slots;
    for (const auto& slot : recipe.slots)
    {
        juce::DynamicObject::Ptr s = new juce::DynamicObject();
        s->setProperty("slotIndex", slot.slotIndex);
        s->setProperty("plugin", internal::mixPluginIdToString(slot.plugin));
        s->setProperty("rationale", slot.rationale);

        juce::DynamicObject::Ptr params = new juce::DynamicObject();
        for (const auto& [key, val] : slot.parameters)
            params->setProperty(key, val);
        s->setProperty("parameters", juce::var(params.get()));
        slots.add(juce::var(s.get()));
    }
    root->setProperty("slots", slots);

    return juce::JSON::toString(juce::var(root.get()), true);
}

bool VibeRecipeJson::fromJson(const juce::String& json, VibeMixRecipe& out, juce::String& error)
{
    const auto parsed = juce::JSON::parse(json);
    if (parsed.isVoid())
    {
        error = "Invalid JSON";
        return false;
    }

    if (auto* obj = parsed.getDynamicObject())
    {
        out = {};
        out.schemaVersion = static_cast<int>(obj->getProperty("schemaVersion"));
        out.title = obj->getProperty("title").toString();
        out.explanation = obj->getProperty("explanation").toString();
        out.target.kind = targetKindFromString(obj->getProperty("target").toString());
        out.target.channelIndex = static_cast<int>(obj->getProperty("channelIndex"));

        if (auto* arr = obj->getProperty("conflicts").getArray())
        {
            for (const auto& v : *arr)
                out.conflicts.push_back(v.toString());
        }

        if (auto* slotArr = obj->getProperty("slots").getArray())
        {
            for (const auto& v : *slotArr)
            {
                if (auto* slotObj = v.getDynamicObject())
                {
                    VibeSlotPreset slot;
                    slot.slotIndex = static_cast<int>(slotObj->getProperty("slotIndex"));
                    slot.plugin = internal::mixPluginIdFromString(slotObj->getProperty("plugin").toString());
                    slot.rationale = slotObj->getProperty("rationale").toString();
                    if (auto* params = slotObj->getProperty("parameters").getDynamicObject())
                    {
                        for (const auto& name : params->getProperties())
                            slot.parameters[name.name.toString()] = static_cast<float>(name.value);
                    }
                    out.slots.push_back(slot);
                }
            }
        }
        return true;
    }

    error = "Expected JSON object";
    return false;
}
} // namespace resonance::audio
