#include <JuceHeader.h>
#include "MainComponent.h"
#include "Model/ProjectState.h"
#include "Audio/AudioEngine.h"
#include "Controller/AppController.h"

class ResonanceApplication : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "Resonance Hybrid DAW"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; }
    bool moreThanOneInstanceAllowed() override { return false; }

    void initialise(const juce::String& commandLine) override
    {
        juce::ignoreUnused(commandLine);

        projectState = std::make_unique<resonance::model::ProjectState>();
        audioEngine = std::make_unique<resonance::audio::AudioEngine>();
        appController = std::make_unique<resonance::controller::AppController>(*projectState, *audioEngine);

        deviceManager = std::make_unique<juce::AudioDeviceManager>();
        deviceManager->initialiseWithDefaultDevices(0, 2);
        appController->attachAudioDeviceManager(*deviceManager);

        mainWindow = std::make_unique<MainWindow>(getApplicationName(), *appController, projectState->getDefaultProjectFile());
    }

    void shutdown() override
    {
        if (appController != nullptr)
            appController->saveProjectToFile(projectState->getDefaultProjectFile());

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
        MainWindow(juce::String name, resonance::controller::AppController& controller, const juce::File& projectFile)
            : DocumentWindow(name,
                             juce::Desktop::getInstance().getDefaultLookAndFeel()
                                 .findColour(juce::ResizableWindow::backgroundColourId),
                             DocumentWindow::allButtons)
            , appController(controller)
            , defaultProjectFile(projectFile)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new resonance::app::MainComponent(controller), true);
            setResizable(true, true);
            centreWithSize(1200, 1040);
            setVisible(true);

            if (defaultProjectFile.existsAsFile())
                appController.loadProjectFromFile(defaultProjectFile);
        }

        void closeButtonPressed() override { juce::JUCEApplication::getInstance()->systemRequestedQuit(); }

        bool keyPressed(const juce::KeyPress& key) override
        {
            if (key.getModifiers().isCommandDown() && key.getKeyCode() == 'S')
            {
                appController.saveProjectToFile(defaultProjectFile);
                return true;
            }
            return DocumentWindow::keyPressed(key);
        }

    private:
        resonance::controller::AppController& appController;
        juce::File defaultProjectFile;
    };

    std::unique_ptr<resonance::model::ProjectState> projectState;
    std::unique_ptr<resonance::audio::AudioEngine> audioEngine;
    std::unique_ptr<resonance::controller::AppController> appController;
    std::unique_ptr<juce::AudioDeviceManager> deviceManager;
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(ResonanceApplication)
