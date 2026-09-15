#include <JuceHeader.h>
#include "MultiScreenManager.h"
#include "Model/ProjectState.h"
#include "Audio/AudioEngine.h"
#include "Controller/AppController.h"

class VMpc2000XLApplication : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "VMPC2000XL"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; }
    bool moreThanOneInstanceAllowed() override { return false; }

    void initialise(const juce::String& commandLine) override
    {
        juce::ignoreUnused(commandLine);

        projectState = std::make_unique<vmpc::model::ProjectState>();
        audioEngine = std::make_unique<vmpc::audio::AudioEngine>();
        appController = std::make_unique<vmpc::controller::AppController>(*projectState, *audioEngine);

        deviceManager = std::make_unique<juce::AudioDeviceManager>();
        deviceManager->initialiseWithDefaultDevices(0, 2);
        appController->attachAudioDeviceManager(*deviceManager);

        multiScreen = std::make_unique<vmpc::app::MultiScreenManager>(*appController);
        multiScreen->openAllModeScreens();
    }

    void shutdown() override
    {
        multiScreen = nullptr;
        if (deviceManager != nullptr && audioEngine != nullptr)
            deviceManager->removeAudioCallback(audioEngine.get());
        appController = nullptr;
        audioEngine = nullptr;
        projectState = nullptr;
        deviceManager = nullptr;
    }

    void systemRequestedQuit() override { quit(); }

private:
    std::unique_ptr<vmpc::model::ProjectState> projectState;
    std::unique_ptr<vmpc::audio::AudioEngine> audioEngine;
    std::unique_ptr<vmpc::controller::AppController> appController;
    std::unique_ptr<juce::AudioDeviceManager> deviceManager;
    std::unique_ptr<vmpc::app::MultiScreenManager> multiScreen;
};

START_JUCE_APPLICATION(VMpc2000XLApplication)
