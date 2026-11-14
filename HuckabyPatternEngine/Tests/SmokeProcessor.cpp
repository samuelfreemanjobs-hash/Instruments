#include <juce_gui_basics/juce_gui_basics.h>

#include "PluginProcessor.h"

int main()
{
    // AudioProcessor/APVTS expect a constructed MessageManager in non-plugin exes.
    juce::ScopedJuceInitialiser_GUI juceSys;

    huckaby::HuckabyAudioProcessor processor;
    processor.prepareToPlay(48000.0, 512);

    juce::AudioBuffer<float> buffer(2, 512);
    juce::MidiBuffer midi;

    processor.processBlock(buffer, midi);

    processor.releaseResources();
    return 0;
}
