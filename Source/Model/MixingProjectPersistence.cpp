#include "MixingProjectPersistence.h"
#include "Audio/Internal/InternalPluginTypes.h"
#include "Audio/PluginSlotLocation.h"

namespace resonance::model
{
namespace
{
juce::ValueTree makeSlotNode(const juce::String& kind,
                             const juce::String& internalId,
                             const juce::MemoryBlock& state)
{
    juce::ValueTree node(slotId);
    node.setProperty("kind", kind, nullptr);
    if (kind == "internal")
        node.setProperty("internalId", internalId, nullptr);
    node.setProperty("state", state.toBase64Encoding(), nullptr);
    return node;
}

void captureSlot(juce::ValueTree& parent, int slotIndex, juce::AudioProcessor* processor)
{
    if (processor == nullptr)
        return;

    juce::MemoryBlock state;
    processor->getStateInformation(state);

    juce::String kind = "external";
    juce::String internalId;

    if (processor->getName().startsWith("Resonance "))
    {
        kind = "internal";
        for (const auto& info : resonance::audio::internal::allMixPlugins())
        {
            if (processor->getName() == info.displayName)
            {
                internalId = resonance::audio::internal::mixPluginIdToString(info.id);
                break;
            }
        }
    }

    auto node = makeSlotNode(kind, internalId, state);
    node.setProperty("index", slotIndex, nullptr);

    if (auto* instance = dynamic_cast<juce::AudioPluginInstance*>(processor))
    {
        const auto& desc = instance->getPluginDescription();
        node.setProperty("pluginName", desc.name, nullptr);
        node.setProperty("pluginFormat", desc.pluginFormatName, nullptr);
        node.setProperty("pluginId", desc.createIdentifierString(), nullptr);
    }

    parent.addChild(node, -1, nullptr);
}
} // namespace

void writeMixingToProject(juce::ValueTree& projectRoot, resonance::audio::PluginHostService& host)
{
    juce::ValueTree mixing(mixingRootId);
    auto& console = host.getMixConsole();

    juce::ValueTree master(masterRackId);
    const auto masterStates = console.getMasterChain().getSlotStates();
    for (int i = 0; i < static_cast<int>(masterStates.size()); ++i)
    {
        if (!masterStates[static_cast<size_t>(i)].loaded)
            continue;
        captureSlot(master, i, console.getMasterChain().getProcessorInSlot(i));
    }
    mixing.addChild(master, -1, nullptr);

    for (int ch = 0; ch < resonance::audio::MixConsole::kNumChannels; ++ch)
    {
        juce::ValueTree channelNode(channelId);
        channelNode.setProperty("index", ch, nullptr);
        channelNode.setProperty("fader", console.getChannelFader(ch), nullptr);
        channelNode.setProperty("mute", console.isChannelMuted(ch), nullptr);

        const auto states = console.getChannelInserts(ch).getSlotStates();
        for (int s = 0; s < static_cast<int>(states.size()); ++s)
        {
            if (!states[static_cast<size_t>(s)].loaded)
                continue;
            captureSlot(channelNode, s, console.getChannelInserts(ch).getProcessorInSlot(s));
        }

        mixing.addChild(channelNode, -1, nullptr);
    }

    projectRoot.removeChild(projectRoot.getChildWithName(mixingRootId), nullptr);
    projectRoot.appendChild(mixing, nullptr);
}

void readMixingFromProject(const juce::ValueTree& projectRoot, resonance::audio::PluginHostService& host)
{
    const auto mixing = projectRoot.getChildWithName(mixingRootId);
    if (!mixing.isValid())
        return;

    auto& console = host.getMixConsole();

    for (int i = 0; i < resonance::audio::PluginSlotChain::kMasterSlots; ++i)
    {
        const resonance::audio::PluginSlotLocation loc { resonance::audio::PluginSlotLocation::Bus::Master, 0, i };
        host.clearSlot(loc);
    }

    for (int ch = 0; ch < resonance::audio::MixConsole::kNumChannels; ++ch)
    {
        for (int s = 0; s < resonance::audio::PluginSlotChain::kChannelInsertSlots; ++s)
        {
            const resonance::audio::PluginSlotLocation loc { resonance::audio::PluginSlotLocation::Bus::Channel, ch, s };
            host.clearSlot(loc);
        }
    }

    const auto master = mixing.getChildWithName(masterRackId);
    for (int i = 0; i < master.getNumChildren(); ++i)
    {
        const auto slot = master.getChild(i);
        if (!slot.hasType(slotId))
            continue;

        const int index = static_cast<int>(slot.getProperty("index", 0));
        resonance::audio::PluginSlotLocation loc { resonance::audio::PluginSlotLocation::Bus::Master, 0, index };

        juce::MemoryBlock state;
        state.fromBase64Encoding(slot.getProperty("state").toString());

        if (slot.getProperty("kind").toString() == "internal")
        {
            const auto id = resonance::audio::internal::mixPluginIdFromString(slot.getProperty("internalId").toString());
            host.loadInternalMixPlugin(loc, id);
            if (auto* proc = host.getProcessorAt(loc))
                proc->setStateInformation(state.getData(), static_cast<int>(state.getSize()));
        }
        else
        {
            const juce::String pluginId = slot.getProperty("pluginId").toString();
            juce::PluginDescription desc;
            bool found = false;
            for (const auto& type : host.getKnownPlugins().getTypes())
            {
                if (type.createIdentifierString() == pluginId)
                {
                    desc = type;
                    found = true;
                    break;
                }
            }

            if (found)
            {
                host.loadPluginIntoSlot(loc, desc, [loc, state, &host](bool ok, const juce::String&) {
                    if (ok)
                        host.restoreExternalSlotFromState(loc, state);
                });
            }
        }
    }

    for (int i = 0; i < mixing.getNumChildren(); ++i)
    {
        const auto channelNode = mixing.getChild(i);
        if (!channelNode.hasType(channelId))
            continue;

        const int ch = static_cast<int>(channelNode.getProperty("index", 0));
        console.setChannelFader(ch, static_cast<float>(channelNode.getProperty("fader", 1.0)));
        console.setChannelMuted(ch, static_cast<bool>(channelNode.getProperty("mute", false)));

        for (int c = 0; c < channelNode.getNumChildren(); ++c)
        {
            const auto slot = channelNode.getChild(c);
            if (!slot.hasType(slotId))
                continue;

            const int slotIndex = static_cast<int>(slot.getProperty("index", 0));
            resonance::audio::PluginSlotLocation loc { resonance::audio::PluginSlotLocation::Bus::Channel, ch, slotIndex };

            juce::MemoryBlock state;
            state.fromBase64Encoding(slot.getProperty("state").toString());

            if (slot.getProperty("kind").toString() == "internal")
            {
                const auto id = resonance::audio::internal::mixPluginIdFromString(slot.getProperty("internalId").toString());
                host.loadInternalMixPlugin(loc, id);
                if (auto* proc = host.getProcessorAt(loc))
                    proc->setStateInformation(state.getData(), static_cast<int>(state.getSize()));
            }
            else
            {
                const juce::String pluginId = slot.getProperty("pluginId").toString();
                juce::PluginDescription desc;
                bool found = false;
                for (const auto& type : host.getKnownPlugins().getTypes())
                {
                    if (type.createIdentifierString() == pluginId)
                    {
                        desc = type;
                        found = true;
                        break;
                    }
                }

                if (found)
                {
                    host.loadPluginIntoSlot(loc, desc, [loc, state, &host](bool ok, const juce::String&) {
                        if (ok)
                            host.restoreExternalSlotFromState(loc, state);
                    });
                }
            }
        }
    }

    console.refreshChannelInsertFlags();
}
} // namespace resonance::model
