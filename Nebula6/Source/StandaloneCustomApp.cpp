#include <JuceHeader.h>
#include <juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h>

#include "PluginProcessor.h"
#include "RenderSmokeTest.h"

namespace juce {

class PlaySeriesStandaloneApp final : public JUCEApplication {
public:
  const String getApplicationName() override { return CharPointer_UTF8(JucePlugin_Name); }
  const String getApplicationVersion() override { return JucePlugin_VersionString; }
  bool moreThanOneInstanceAllowed() override { return true; }

  void initialise(const String &commandLine) override {
    if (commandLine.contains("--render-test")) {
      PlaySeriesTemplateAudioProcessor proc;
      const File out =
          File::getSpecialLocation(File::tempDirectory).getChildFile("play_render_preview.wav");
      const auto result = play_series::runRenderSmokeTest(proc, out);
      Logger::writeToLog(result.success
                             ? "Render smoke OK: " + String(result.peakDbfs, 2) + " dBFS"
                             : "Render smoke failed: " + result.error);
      setApplicationReturnValue(result.success ? 0 : 2);
      quit();
      return;
    }

    PropertiesFile::Options options;
    options.applicationName = getApplicationName();
    options.filenameSuffix = ".settings";
    appProperties.setStorageParameters(options);

    mainWindow.reset(new StandaloneFilterWindow(
        getApplicationName(),
        LookAndFeel::getDefaultLookAndFeel().findColour(ResizableWindow::backgroundColourId),
        appProperties.getUserSettings(), false, {}, nullptr, {}, false));
    mainWindow->setVisible(true);
  }

  void shutdown() override {
    mainWindow = nullptr;
    appProperties.saveIfNeeded();
  }

  void systemRequestedQuit() override { quit(); }

private:
  ApplicationProperties appProperties;
  std::unique_ptr<StandaloneFilterWindow> mainWindow;
};

} // namespace juce

juce::JUCEApplicationBase *juce_CreateApplication() {
  return new juce::PlaySeriesStandaloneApp();
}