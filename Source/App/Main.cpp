#include <JuceHeader.h>
#include "MainComponent.h"
#include "Model/ProjectState.h"
#include "Audio/AudioEngine.h"
#include "Controller/AppController.h"

class VMpc2000XLApplication : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "VMPC2000XL Hybrid DAW"; }
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

        mainWindow = std::make_unique<MainWindow>(getApplicationName(), *appController);
    }

    void shutdown() override
    {
        mainWindow = nullptr;
        if (deviceManager != nullptr)
            deviceManager->removeAudioCallback(audioEngine.get());
        appController = nullptr;
        audioEngine = nullptr;
        projectState = nullptr;
        deviceManager = nullptr;
    }

    void systemRequestedQuit() override { quit(); }

private:
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name, vmpc::controller::AppController& controller)
            : DocumentWindow(name,
                             juce::Desktop::getInstance().getDefaultLookAndFeel()
                                 .findColour(juce::ResizableWindow::backgroundColourId),
                             DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new vmpc::app::MainComponent(controller), true);
            setResizable(true, true);
            centreWithSize(1200, 900);
            setVisible(true);
        }

        void closeButtonPressed() override { juce::JUCEApplication::getInstance()->systemRequestedQuit(); }
    };

    std::unique_ptr<vmpc::model::ProjectState> projectState;
    std::unique_ptr<vmpc::audio::AudioEngine> audioEngine;
    std::unique_ptr<vmpc::controller::AppController> appController;
    std::unique_ptr<juce::AudioDeviceManager> deviceManager;
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(VMpc2000XLApplication)
