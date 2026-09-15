#include "PluginHostService.h"
#include "AudioEngine.h"

namespace vmpc::audio
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

    audioEngine.setPluginChain(&chain);
    startBackgroundScan();
}

PluginHostService::~PluginHostService()
{
    knownPlugins.removeChangeListener(this);
    audioEngine.setPluginChain(nullptr);

    for (auto& window : editorWindows)
        window.reset();

    chain.releaseResources();
    for (int i = 0; i < PluginSlotChain::kNumSlots; ++i)
        chain.setPluginInSlot(i, nullptr);
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
        .getChildFile("VMPC2000XL")
        .getChildFile("known_plugins.xml");
}

juce::FileSearchPath PluginHostService::defaultPluginSearchPath() const
{
    juce::FileSearchPath paths;

#if JUCE_WINDOWS
    paths.add(juce::File::getSpecialLocation(juce::File::commonApplicationDataDirectory)
                  .getChildFile("VST3"));
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

void PluginHostService::loadPluginIntoSlot(int slotIndex,
                                           const juce::PluginDescription& description,
                                           std::function<void(bool, const juce::String&)> callback)
{
    if (slotIndex < 0 || slotIndex >= PluginSlotChain::kNumSlots)
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
        [this, slotIndex, callback](std::unique_ptr<juce::AudioPluginInstance> instance, const juce::String& error) {
            if (instance == nullptr)
            {
                if (callback)
                    callback(false, error);
                return;
            }

            chain.setPluginInSlot(slotIndex, std::move(instance));
            notifySlotsChanged();

            if (callback)
                callback(true, {});
        });
}

void PluginHostService::clearSlot(int slotIndex)
{
    hideEditorForSlot(slotIndex);
    chain.setPluginInSlot(slotIndex, nullptr);
    notifySlotsChanged();
}

void PluginHostService::showEditorForSlot(int slotIndex)
{
    if (slotIndex < 0 || slotIndex >= PluginSlotChain::kNumSlots)
        return;

    auto* plugin = chain.getPluginInSlot(slotIndex);
    if (plugin == nullptr)
        return;

    if (auto* editor = plugin->getActiveEditor())
    {
        editor->toFront(true);
        return;
    }

    if (auto* created = plugin->createEditorIfNeeded())
    {
        editorWindows[static_cast<size_t>(slotIndex)] =
            std::make_unique<PluginEditorWindow>(plugin->getName(), created);
    }
}

void PluginHostService::hideEditorForSlot(int slotIndex)
{
    if (slotIndex < 0 || slotIndex >= PluginSlotChain::kNumSlots)
        return;

    editorWindows[static_cast<size_t>(slotIndex)].reset();
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
} // namespace vmpc::audio
