#pragma once

#include <JuceHeader.h>
#include "MixConsole.h"
#include "PluginSlotLocation.h"
#include "Internal/InternalPluginTypes.h"
#include "VibeMixingTypes.h"
#include <functional>
#include <unordered_map>
#include <vector>

namespace resonance::audio
{
class AudioEngine;

/** Message-thread plugin discovery, loading, editors, and project mixing state. */
class PluginHostService : private juce::ChangeListener
{
public:
    struct Listener
    {
        virtual ~Listener() = default;
        virtual void pluginSlotsChanged() = 0;
        virtual void pluginScanFinished() = 0;
    };

    struct SavedVibeEntry
    {
        juce::String name;
        VibeMixRecipe recipe;
    };

    explicit PluginHostService(AudioEngine& engine);
    ~PluginHostService() override;

    MixConsole& getMixConsole() noexcept { return mixConsole; }
    PluginSlotChain& getMasterChain() noexcept { return mixConsole.getMasterChain(); }

    juce::KnownPluginList& getKnownPlugins() noexcept { return knownPlugins; }
    juce::AudioPluginFormatManager& getFormatManager() noexcept { return formatManager; }

    void startBackgroundScan();
    bool isScanning() const noexcept { return scanning; }

    void loadPluginIntoSlot(PluginSlotLocation location,
                            const juce::PluginDescription& description,
                            std::function<void(bool success, const juce::String& error)> callback);

    void loadInternalMixPlugin(PluginSlotLocation location, internal::MixPluginId id);
    void restoreExternalSlotFromState(PluginSlotLocation location, const juce::MemoryBlock& state);

    VibeMixInterpretResult interpretVibeMix(const juce::String& userText, const VibeMixTarget& target);
    bool applyVibeRecipe(const VibeMixRecipe& recipe, bool recordUndoSnapshot = true);
    /** @deprecated Use interpretVibeMix + applyVibeRecipe */
    VibeMixInterpretResult applyVibeMixFromBrief(const juce::String& userText);

    juce::ValueTree captureMixingSnapshot();
    void restoreMixingSnapshot(const juce::ValueTree& snapshot);
    bool undoLastVibeApply();
    bool swapVibeAB();
    bool hasVibeUndo() const noexcept { return vibeUndoSnapshot.isValid(); }
    bool hasVibeABCompare() const noexcept { return vibeCompareA.isValid() && vibeCompareB.isValid(); }

    const juce::StringArray& getVibeBriefHistory() const noexcept { return vibeBriefHistory; }
    void setVibeBriefHistory(const juce::StringArray& briefs);
    void appendVibeBrief(const juce::String& brief);

    const std::vector<SavedVibeEntry>& getSavedVibeRecipes() const noexcept { return savedVibeRecipes; }
    void setSavedVibeRecipes(const std::vector<SavedVibeEntry>& entries);
    void saveNamedVibe(const juce::String& name, const VibeMixRecipe& recipe);

    static PluginSlotLocation locationForVibeSlot(const VibeMixTarget& target, int slotIndex) noexcept;

    void clearSlot(PluginSlotLocation location);
    void showEditorForSlot(PluginSlotLocation location);
    void hideEditorForSlot(PluginSlotLocation location);

    juce::AudioProcessor* getProcessorAt(PluginSlotLocation location) noexcept;

    void setMixingChangedCallback(std::function<void()> callback) { mixingChanged = std::move(callback); }

    void addListener(Listener* listener);
    void removeListener(Listener* listener);

    juce::File getPluginListCacheFile() const;

private:
    PluginSlotChain& chainFor(PluginSlotLocation location);
    bool isValidLocation(const PluginSlotLocation& location) const noexcept;
    void clearVibeTargetSlots(const VibeMixTarget& target);

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    juce::FileSearchPath defaultPluginSearchPath() const;
    void notifySlotsChanged();
    void notifyScanFinished();
    void markMixingDirty();

    AudioEngine& audioEngine;
    MixConsole mixConsole;
    juce::AudioPluginFormatManager formatManager;
    juce::KnownPluginList knownPlugins;
    std::unordered_map<uint32_t, std::unique_ptr<juce::DocumentWindow>> editorWindows;
    bool scanning = false;
    juce::ListenerList<Listener> listeners;
    std::function<void()> mixingChanged;

    juce::ValueTree vibeUndoSnapshot;
    juce::ValueTree vibeCompareA;
    juce::ValueTree vibeCompareB;
    bool vibeAbShowingB = false;
    juce::StringArray vibeBriefHistory;
    std::vector<SavedVibeEntry> savedVibeRecipes;
};
} // namespace resonance::audio
