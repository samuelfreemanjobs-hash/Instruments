#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "Parameters/ParameterIds.h"
#include "UI/UiLayout.h"

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <array>
#include <cstdlib>
#include <iostream>


int main (int argc, char** argv)
{
    const juce::String outPath = argc > 1 ? juce::String (argv[1])
                                          : juce::String ("/opt/cursor/artifacts/junova-editor-screenshot.png");

    JunovaXAudioProcessor processor;
    processor.prepareToPlay (48000.0, 512);
    processor.applyFactoryPreset (0);
    if (auto* voice = processor.getApvts().getParameter (junovax::ParameterIDs::voiceMode))
        voice->setValueNotifyingHost (1.0f);

    juce::AudioBuffer<float> buffer (2, 512);
    juce::MidiBuffer midi;
    midi.addEvent (juce::MidiMessage::noteOn (1, 60, static_cast<juce::uint8> (100)), 0);
    midi.addEvent (juce::MidiMessage::noteOn (1, 64, static_cast<juce::uint8> (95)), 0);
    midi.addEvent (juce::MidiMessage::noteOn (1, 67, static_cast<juce::uint8> (90)), 0);
    for (int b = 0; b < 120; ++b)
    {
        juce::MidiBuffer slice;
        if (b == 0)
            slice = midi;
        processor.processBlock (buffer, slice);
    }

    auto editor = std::unique_ptr<juce::AudioProcessorEditor> (processor.createEditor());
    editor->setBounds (0, 0, junovax::ui::Layout::editorWidth, junovax::ui::Layout::editorHeight);
    editor->resized();
    editor->toFront (true);

    std::array<float, 256> scopeScratch{};
    processor.copyScopeSamples (scopeScratch.data(), static_cast<int> (scopeScratch.size()));

    if (auto* junovaEditor = dynamic_cast<JunovaXAudioProcessorEditor*> (editor.get()))
        junovaEditor->syncForSnapshot();

    editor->repaint();
    juce::Thread::sleep (80);
    editor->repaint();

    const auto image = editor->createComponentSnapshot (editor->getLocalBounds(), true, 1.0f);
    juce::PNGImageFormat png;
    juce::File file (outPath);
    file.getParentDirectory().createDirectory();
    std::unique_ptr<juce::FileOutputStream> stream (file.createOutputStream());
    if (stream == nullptr || ! png.writeImageToStream (image, *stream))
    {
        std::cerr << "Failed to write " << outPath << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "Wrote " << outPath << '\n';
    return EXIT_SUCCESS;
}