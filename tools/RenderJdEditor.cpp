#include "PluginEditor.h"
#include "PluginProcessor.h"

#include <juce_gui_basics/juce_gui_basics.h>

#include <cstdio>

int main (int argc, char** argv)
{
    juce::ScopedJuceInitialiser_GUI juceInit;

    const juce::File out (argc > 1 ? juce::String (argv[1])
                                   : "/opt/cursor/artifacts/jd-upgraded-editor.png");

    JDUpgradedAudioProcessor proc;
    proc.prepareToPlay (44100.0, 512);
    std::unique_ptr<juce::AudioProcessorEditor> editor (proc.createEditor());
    if (editor == nullptr)
    {
        std::fprintf (stderr, "createEditor failed\n");
        return 1;
    }

    editor->setBounds (0, 0, editor->getWidth(), editor->getHeight());
    juce::Image img (juce::Image::ARGB, editor->getWidth(), editor->getHeight(), true);
    juce::Graphics g (img);
    editor->paintEntireComponent (g, true);

    out.getParentDirectory().createDirectory();
    if (out.existsAsFile())
        out.deleteFile();

    juce::FileOutputStream stream (out);
    if (! stream.openedOk())
    {
        std::fprintf (stderr, "cannot write %s\n", out.getFullPathName().toRawUTF8());
        return 1;
    }

    juce::PNGImageFormat png;
    if (! png.writeImageToStream (img, stream))
    {
        std::fprintf (stderr, "PNG write failed\n");
        return 1;
    }

    std::printf ("Wrote %s\n", out.getFullPathName().toRawUTF8());
    return 0;
}