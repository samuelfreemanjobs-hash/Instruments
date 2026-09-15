#pragma once

#include <JuceHeader.h>
#include "PluginSlotChain.h"
#include "Internal/InternalPluginTypes.h"
#include "VibeMixingTypes.h"
#include <functional>

namespace vmpc::audio
{
class AudioEngine;

/** Message-thread plugin discovery, loading, and editor windows. */
class PluginHostService : private juce::ChangeListener
{
public:
    struct Listener
    {
        virtual ~Listener() = default;
        virtual void pluginSlotsChanged() = 0;
        virtual void pluginScanFinished() = 0;
    };

    explicit PluginHostService(AudioEngine& engine);
    ~PluginHostService() override;

    PluginSlotChain& getChain() noexcept { return chain; }
    juce::KnownPluginList& getKnownPlugins() noexcept { return knownPlugins; }
    juce::AudioPluginFormatManager& getFormatManager() noexcept { return formatManager; }

    void startBackgroundScan();
    bool isScanning() const noexcept { return scanning; }

    void loadPluginIntoSlot(int slotIndex,
                            const juce::PluginDescription& description,
                            std::function<void(bool success, const juce::String& error)> callback);

    void loadInternalMixPlugin(int slotIndex, internal::MixPluginId id);

    /** Vibe Mixing: type a brief → load internal chain + preset on the rack. */
    VibeMixInterpretResult applyVibeMixFromBrief(const juce::String& userText);

    void clearSlot(int slotIndex);
    void showEditorForSlot(int slotIndex);
    void hideEditorForSlot(int slotIndex);

    void addListener(Listener* listener);
    void removeListener(Listener* listener);

    juce::File getPluginListCacheFile() const;

private:
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    juce::FileSearchPath defaultPluginSearchPath() const;
    void notifySlotsChanged();
    void notifyScanFinished();

    AudioEngine& audioEngine;
    PluginSlotChain chain;
    juce::AudioPluginFormatManager formatManager;
    juce::KnownPluginList knownPlugins;
    std::array<std::unique_ptr<juce::DocumentWindow>, PluginSlotChain::kNumSlots> editorWindows;
    bool scanning = false;
    juce::ListenerList<Listener> listeners;
};
} // namespace vmpc::audio
