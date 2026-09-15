#include "PluginHostService.h"
#include "AudioEngine.h"
#include "VibeMixEngine.h"
#include "Internal/InternalPluginParams.h"
#include "Model/MixingProjectPersistence.h"

namespace resonance::audio
{
namespace
{
class PluginEditorWindow : public juce::DocumentWindow
{
public:
    PluginEditorWindow(const juce::String& title, juce::AudioProcessorEditor* editor)
        : DocumentWindow(title, juce::Colour(0xff1a1b21), DocumentWindow::closeButton)
    {
        setUsingNativeTitleBar(true);
        setContentOwned(editor, true);
        setResizable(true, true);
        centreWithSize(juce::jmax(480, editor->getWidth()), juce::jmax(360, editor->getHeight()));
        setVisible(true);
    }

    void closeButtonPressed() override { setVisible(false); }
};
} // namespace

PluginHostService::PluginHostService(AudioEngine& engine)
    : audioEngine(engine)
{
    formatManager.addDefaultFormats();
    knownPlugins.addChangeListener(this);

    if (auto cache = getPluginListCacheFile(); cache.existsAsFile())
        knownPlugins.recreateFromXml(*juce::parseXML(cache));

    audioEngine.setMixConsole(&mixConsole);
    mixConsole.refreshChannelInsertFlags();
    startBackgroundScan();
}

PluginHostService::~PluginHostService()
{
    knownPlugins.removeChangeListener(this);
    audioEngine.setMixConsole(nullptr);
    editorWindows.clear();
    mixConsole.releaseResources();
}

PluginSlotChain& PluginHostService::chainFor(PluginSlotLocation location)
{
    if (location.bus == PluginSlotLocation::Bus::Master)
        return mixConsole.getMasterChain();
    return mixConsole.getChannelInserts(location.channelIndex);
}

bool PluginHostService::isValidLocation(const PluginSlotLocation& location) const noexcept
{
    return location.isValidForMaster() || location.isValidForChannel();
}

juce::AudioProcessor* PluginHostService::getProcessorAt(PluginSlotLocation location) noexcept
{
    if (!isValidLocation(location))
        return nullptr;
    return chainFor(location).getProcessorInSlot(location.slotIndex);
}

void PluginHostService::addListener(Listener* listener)
{
    listeners.add(listener);
}

void PluginHostService::removeListener(Listener* listener)
{
    listeners.remove(listener);
}

juce::File PluginHostService::getPluginListCacheFile() const
{
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("Instruments")
        .getChildFile("Resonance")
        .getChildFile("known_plugins.xml");
}

juce::FileSearchPath PluginHostService::defaultPluginSearchPath() const
{
    juce::FileSearchPath paths;

#if JUCE_WINDOWS
    paths.add(juce::File::getSpecialLocation(juce::File::commonApplicationDataDirectory).getChildFile("VST3"));
#elif JUCE_MAC
    paths.add(juce::File("/Library/Audio/Plug-Ins/VST3"));
    paths.add(juce::File("~/Library/Audio/Plug-Ins/VST3"));
#else
    paths.add(juce::File("/usr/lib/vst3"));
    paths.add(juce::File("/usr/local/lib/vst3"));
    paths.add(juce::File("~/.vst3"));
#endif

    paths.add(juce::File::getSpecialLocation(juce::File::userHomeDirectory).getChildFile(".vst3"));

#if JUCE_LINUX && JUCE_PLUGINHOST_LV2
    paths.add(juce::File("/usr/lib/lv2"));
    paths.add(juce::File("~/.lv2"));
#endif

    return paths;
}

void PluginHostService::startBackgroundScan()
{
    if (scanning)
        return;

    scanning = true;
    const auto paths = defaultPluginSearchPath();

    juce::Thread::launch([this, paths]() {
        for (int i = formatManager.getNumFormats(); --i >= 0;)
        {
            auto* format = formatManager.getFormat(i);
            if (format == nullptr)
                continue;

            juce::PluginDirectoryScanner scanner(knownPlugins, *format, paths, true, juce::File(), true);
            juce::String name;
            while (scanner.scanNextFile(true, name))
            {
            }
        }

        juce::MessageManager::callAsync([this]() {
            if (auto xml = knownPlugins.createXml())
            {
                const auto cacheFile = getPluginListCacheFile();
                cacheFile.getParentDirectory().createDirectory();
                xml->writeTo(cacheFile);
            }

            scanning = false;
            notifyScanFinished();
        });
    });
}

void PluginHostService::loadPluginIntoSlot(PluginSlotLocation location,
                                           const juce::PluginDescription& description,
                                           std::function<void(bool, const juce::String&)> callback)
{
    if (!isValidLocation(location))
    {
        if (callback)
            callback(false, "Invalid slot");
        return;
    }

    const double sr = audioEngine.getSampleRate();
    const int block = audioEngine.getBlockSize();

    formatManager.createPluginInstanceAsync(
        description,
        sr > 0.0 ? sr : 44100.0,
        block > 0 ? block : 512,
        [this, location, callback](std::unique_ptr<juce::AudioPluginInstance> instance, const juce::String& error) {
            if (instance == nullptr)
            {
                if (callback)
                    callback(false, error);
                return;
            }

            std::unique_ptr<juce::AudioProcessor> processor { std::move(instance) };
            chainFor(location).setProcessorInSlot(location.slotIndex, std::move(processor));
            mixConsole.refreshChannelInsertFlags();
            notifySlotsChanged();
            markMixingDirty();

            if (callback)
                callback(true, {});
        });
}

void PluginHostService::restoreExternalSlotFromState(PluginSlotLocation location, const juce::MemoryBlock& state)
{
    if (!isValidLocation(location) || state.getSize() == 0)
        return;

    if (auto* processor = getProcessorAt(location))
        processor->setStateInformation(state.getData(), static_cast<int>(state.getSize()));
}

PluginSlotLocation PluginHostService::locationForVibeSlot(const VibeMixTarget& target, int slotIndex) noexcept
{
    if (target.usesMasterBus())
        return { PluginSlotLocation::Bus::Master, 0, slotIndex };

    return { PluginSlotLocation::Bus::Channel, target.resolvedChannelIndex(), slotIndex };
}

void PluginHostService::clearVibeTargetSlots(const VibeMixTarget& target)
{
    if (target.usesMasterBus())
    {
        for (int i = 0; i < PluginSlotChain::kMasterSlots; ++i)
        {
            const auto loc = locationForVibeSlot(target, i);
            hideEditorForSlot(loc);
            clearSlot(loc);
        }
        return;
    }

    for (int i = 0; i < PluginSlotChain::kChannelInsertSlots; ++i)
    {
        const auto loc = locationForVibeSlot(target, i);
        hideEditorForSlot(loc);
        clearSlot(loc);
    }
}

VibeMixInterpretResult PluginHostService::interpretVibeMix(const juce::String& userText, const VibeMixTarget& target)
{
    return VibeMixEngine::interpretUserBrief(userText, target);
}

bool PluginHostService::applyVibeRecipe(const VibeMixRecipe& recipe, bool recordUndoSnapshot)
{
    if (recipe.slots.empty())
        return false;

    if (recordUndoSnapshot)
    {
        vibeUndoSnapshot = captureMixingSnapshot();
        if (!vibeCompareA.isValid())
            vibeCompareA = vibeUndoSnapshot.createCopy();
    }

    clearVibeTargetSlots(recipe.target);

    for (const auto& slotPreset : recipe.slots)
    {
        const auto loc = locationForVibeSlot(recipe.target, slotPreset.slotIndex);
        loadInternalMixPlugin(loc, slotPreset.plugin);

        if (auto* processor = getProcessorAt(loc))
            internal::applyRawParameterMap(*processor, slotPreset.parameters);
    }

    vibeCompareB = captureMixingSnapshot();
    vibeAbShowingB = true;

    notifySlotsChanged();
    markMixingDirty();
    return true;
}

VibeMixInterpretResult PluginHostService::applyVibeMixFromBrief(const juce::String& userText)
{
    auto result = interpretVibeMix(userText, {});
    if (result.success)
        applyVibeRecipe(result.recipe);
    return result;
}

juce::ValueTree PluginHostService::captureMixingSnapshot()
{
    return resonance::model::captureMixingSubtree(*this);
}

void PluginHostService::restoreMixingSnapshot(const juce::ValueTree& snapshot)
{
    if (!snapshot.isValid())
        return;

    resonance::model::applyMixingSubtree(snapshot, *this);
    notifySlotsChanged();
    markMixingDirty();
}

bool PluginHostService::undoLastVibeApply()
{
    if (!vibeUndoSnapshot.isValid())
        return false;

    restoreMixingSnapshot(vibeUndoSnapshot);
    vibeUndoSnapshot = {};
    return true;
}

bool PluginHostService::swapVibeAB()
{
    if (!vibeCompareA.isValid() || !vibeCompareB.isValid())
        return false;

    vibeAbShowingB = !vibeAbShowingB;
    restoreMixingSnapshot(vibeAbShowingB ? vibeCompareB : vibeCompareA);
    return true;
}

void PluginHostService::setVibeBriefHistory(const juce::StringArray& briefs)
{
    vibeBriefHistory = briefs;
}

void PluginHostService::appendVibeBrief(const juce::String& brief)
{
    const auto trimmed = brief.trim();
    if (trimmed.isEmpty())
        return;

    if (vibeBriefHistory.size() > 0 && vibeBriefHistory[0] == trimmed)
        return;

    vibeBriefHistory.removeString(trimmed);
    vibeBriefHistory.insert(0, trimmed);

    while (vibeBriefHistory.size() > 24)
        vibeBriefHistory.remove(vibeBriefHistory.size() - 1);

    markMixingDirty();
}

void PluginHostService::setSavedVibeRecipes(const std::vector<SavedVibeEntry>& entries)
{
    savedVibeRecipes = entries;
}

void PluginHostService::saveNamedVibe(const juce::String& name, const VibeMixRecipe& recipe)
{
    SavedVibeEntry entry;
    entry.name = name.trim().isEmpty() ? recipe.title : name.trim();
    entry.recipe = recipe;

    for (size_t i = 0; i < savedVibeRecipes.size(); ++i)
    {
        if (savedVibeRecipes[i].name == entry.name)
        {
            savedVibeRecipes[i] = entry;
            markMixingDirty();
            return;
        }
    }

    savedVibeRecipes.push_back(std::move(entry));
    markMixingDirty();
}

void PluginHostService::loadInternalMixPlugin(PluginSlotLocation location, internal::MixPluginId id)
{
    if (!isValidLocation(location))
        return;

    hideEditorForSlot(location);

    auto processor = internal::createMixPlugin(id);
    if (processor == nullptr)
        return;

    chainFor(location).setProcessorInSlot(location.slotIndex, std::move(processor));
    mixConsole.refreshChannelInsertFlags();
    notifySlotsChanged();
    markMixingDirty();
}

void PluginHostService::clearSlot(PluginSlotLocation location)
{
    if (!isValidLocation(location))
        return;

    hideEditorForSlot(location);
    chainFor(location).setProcessorInSlot(location.slotIndex, nullptr);
    mixConsole.refreshChannelInsertFlags();
    notifySlotsChanged();
    markMixingDirty();
}

void PluginHostService::showEditorForSlot(PluginSlotLocation location)
{
    if (!isValidLocation(location))
        return;

    auto* plugin = getProcessorAt(location);
    if (plugin == nullptr)
        return;

    if (auto* editor = plugin->getActiveEditor())
    {
        editor->toFront(true);
        return;
    }

    if (auto* created = plugin->createEditorIfNeeded())
    {
        editorWindows[location.editorKey()] = std::make_unique<PluginEditorWindow>(plugin->getName(), created);
    }
}

void PluginHostService::hideEditorForSlot(PluginSlotLocation location)
{
    editorWindows.erase(location.editorKey());
}

void PluginHostService::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    juce::ignoreUnused(source);
    notifySlotsChanged();
}

void PluginHostService::notifySlotsChanged()
{
    listeners.call([](Listener& l) { l.pluginSlotsChanged(); });
}

void PluginHostService::notifyScanFinished()
{
    listeners.call([](Listener& l) { l.pluginScanFinished(); });
}

void PluginHostService::markMixingDirty()
{
    if (mixingChanged)
        mixingChanged();
}
} // namespace resonance::audio
