#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "UI/LayoutConstants.h"

#include <cstdlib>
#include <iostream>

int main()
{
    juce::ScopedJuceInitialiser_GUI juceInit;
    Wave9090AudioProcessor proc;
    proc.setCurrentProgram (8);
    Wave9090AudioProcessorEditor editor (proc);
    editor.syncFactoryProgramUi();

    const juce::File out = juce::File::getSpecialLocation (juce::File::tempDirectory)
                               .getChildFile ("wave9090-editor.png");

    const auto artifactPath = juce::File ("/opt/cursor/artifacts/wave9090-editor.png");
    artifactPath.getParentDirectory().createDirectory();

    const juce::File browserShot = juce::File ("/opt/cursor/artifacts/wave9090-preset-browser.png");
    editor.renderEditorPng (browserShot);
    editor.renderEditorPng (artifactPath);
    editor.renderEditorPng (out);

    if (! artifactPath.existsAsFile() || artifactPath.getSize() < 8000)
    {
        std::cerr << "Editor screenshot missing or too small\n";
        return EXIT_FAILURE;
    }

    std::cout << "Wave9090EditorScreenshot OK: " << artifactPath.getFullPathName() << '\n';
    return EXIT_SUCCESS;
}