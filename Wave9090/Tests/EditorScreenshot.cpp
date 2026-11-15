#include "PluginEditor.h"
#include "PluginProcessor.h"

#include <cstdlib>
#include <iostream>

int main()
{
    juce::ScopedJuceInitialiser_GUI juceInit;
    Wave9090AudioProcessor proc;
    Wave9090AudioProcessorEditor editor (proc);
    editor.setSize (920, 560);

    const juce::File out = juce::File::getSpecialLocation (juce::File::tempDirectory)
                               .getChildFile ("wave9090-editor.png");

    const auto artifactPath = juce::File ("/opt/cursor/artifacts/wave9090-editor.png");
    artifactPath.getParentDirectory().createDirectory();

    editor.renderEditorPng (artifactPath);
    editor.renderEditorPng (out);

    if (! artifactPath.existsAsFile() || artifactPath.getSize() < 1000)
    {
        std::cerr << "Editor screenshot missing or too small\n";
        return EXIT_FAILURE;
    }

    std::cout << "Wave9090EditorScreenshot OK: " << artifactPath.getFullPathName() << '\n';
    return EXIT_SUCCESS;
}