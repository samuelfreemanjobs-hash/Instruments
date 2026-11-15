#include <cmath>
#include <iostream>

#include <juce_gui_basics/juce_gui_basics.h>

#include "PluginProcessor.h"

namespace
{

float bufferPeak(const juce::AudioBuffer<float>& buffer)
{
    float peak = 0.0f;
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        peak = juce::jmax(peak, buffer.getMagnitude(ch, 0, buffer.getNumSamples()));
    return peak;
}

std::shared_ptr<huckaby::SampleData> makeTestClick(double sampleRate)
{
    constexpr int length = 4096;
    juce::AudioBuffer<float> buf(1, length);
    buf.clear();

    for (int i = 0; i < length; ++i)
    {
        const float t = static_cast<float>(i) / static_cast<float>(sampleRate);
        const float env = std::exp(-t * 40.0f);
        buf.setSample(0, i, 0.85f * env * std::sin(juce::MathConstants<float>::twoPi * 440.0f * t));
    }

    return std::make_shared<huckaby::SampleData>(std::move(buf), sampleRate, "qa_click");
}

void setParamNormalized(huckaby::HuckabyAudioProcessor& processor, const char* id, float norm)
{
    if (auto* p = processor.getApvts().getParameter(id))
        p->setValueNotifyingHost(juce::jlimit(0.0f, 1.0f, norm));
}

} // namespace

int main()
{
    juce::ScopedJuceInitialiser_GUI juceSys;

    constexpr double sampleRate = 48000.0;
    constexpr int blockSize = 512;

    huckaby::HuckabyAudioProcessor processor;
    processor.prepareToPlay(sampleRate, blockSize);

    processor.assignSampleForTest(0, 0, makeTestClick(sampleRate));
    processor.setPatternBits(0, 1u);

    setParamNormalized(processor, "internal_transport", 1.0f);
    setParamNormalized(processor, "internal_play", 1.0f);
    setParamNormalized(processor, "master_gain_db", 1.0f);

    juce::AudioBuffer<float> buffer(2, blockSize);
    juce::MidiBuffer midi;
    float audioPeak = 0.0f;

    for (int block = 0; block < 24; ++block)
    {
        buffer.clear();
        processor.processBlock(buffer, midi);
        audioPeak = juce::jmax(audioPeak, bufferPeak(buffer));
    }

    midi.addEvent(juce::MidiMessage::noteOn(1, 36, static_cast<juce::uint8>(100)), 0);
    for (int block = 0; block < 4; ++block)
    {
        buffer.clear();
        processor.processBlock(buffer, midi);
        midi.clear();
    }

    processor.releaseResources();

    if (audioPeak < 1.0e-4f)
    {
        std::cerr << "Huckaby audio QA failed: expected non-silent sequencer output, peak=" << audioPeak << '\n';
        return 1;
    }

    std::cout << "Huckaby audio QA passed (peak=" << audioPeak << ")\n";
    return 0;
}