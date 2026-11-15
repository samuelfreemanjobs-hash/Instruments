#include "DSP/SynthEngine.h"
#include "Presets/PresetParams.h"

#include <juce_audio_formats/juce_audio_formats.h>

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace
{
bool writeWav (const juce::File& file, const juce::AudioBuffer<float>& buffer, double sampleRate)
{
    juce::WavAudioFormat format;
    std::unique_ptr<juce::FileOutputStream> stream (file.createOutputStream());
    if (stream == nullptr)
        return false;
    std::unique_ptr<juce::AudioFormatWriter> writer (
        format.createWriterFor (stream.get(), sampleRate, static_cast<unsigned int> (buffer.getNumChannels()), 16, {}, 0));
    if (writer == nullptr)
        return false;
    stream.release();
    return writer->writeFromAudioSampleBuffer (buffer, 0, buffer.getNumSamples());
}

void renderEngine (junovax::dsp::SynthEngine& engine,
                   junovax::dsp::RuntimeParams params,
                   int midiNote,
                   double seconds,
                   double sampleRate,
                   juce::AudioBuffer<float>& out)
{
    engine.prepare (sampleRate, 512);
    engine.setParams (params);
    engine.reset();

    const int totalSamples = static_cast<int> (seconds * sampleRate);
    out.setSize (2, totalSamples, false, true, true);
    out.clear();
    juce::MidiBuffer midi;
    midi.addEvent (juce::MidiMessage::noteOn (1, midiNote, static_cast<juce::uint8> (110)), 0);

    juce::AudioBuffer<float> block (2, 512);
    int written = 0;
    while (written < totalSamples)
    {
        const int n = std::min (512, totalSamples - written);
        block.setSize (2, n, false, false, true);
        block.clear();
        juce::MidiBuffer blockMidi;
        if (written == 0)
            blockMidi = midi;
        engine.render (block, blockMidi);
        for (int ch = 0; ch < 2; ++ch)
            out.copyFrom (ch, written, block, ch, 0, n);
        written += n;
    }
    midi.addEvent (juce::MidiMessage::noteOff (1, midiNote), totalSamples - 1);
}

void fftMag (const std::vector<float>& mono, std::vector<float>& mag)
{
    const int n = static_cast<int> (mono.size());
    mag.assign (static_cast<std::size_t> (n / 2), 0.0f);
    for (int k = 0; k < n / 2; ++k)
    {
        double re = 0.0;
        double im = 0.0;
        for (int i = 0; i < n; ++i)
        {
            const double phase = -2.0 * 3.141592653589793 * static_cast<double> (k) * static_cast<double> (i)
                               / static_cast<double> (n);
            re += static_cast<double> (mono[static_cast<std::size_t> (i)]) * std::cos (phase);
            im += static_cast<double> (mono[static_cast<std::size_t> (i)]) * std::sin (phase);
        }
        mag[static_cast<std::size_t> (k)] = static_cast<float> (std::sqrt (re * re + im * im) / static_cast<double> (n));
    }
}

void writePpmSpectrum (const juce::File& file, const std::vector<float>& mag, double sampleRate, const juce::String& label)
{
    const int w = 1024;
    const int h = 512;
    std::ofstream out (file.getFullPathName().toStdString(), std::ios::binary);
    out << "P6\n" << w << " " << h << "\n255\n";
    const int maxBin = static_cast<int> (mag.size());
    const int plotBins = std::min (maxBin, 4096);
    std::vector<unsigned char> row (static_cast<std::size_t> (w * 3));
    for (int y = 0; y < h; ++y)
    {
        row.assign (static_cast<std::size_t> (w * 3), 8);
        const float yNorm = 1.0f - static_cast<float> (y) / static_cast<float> (h);
        const float db = -90.0f + yNorm * 90.0f;
        const float thresh = std::pow (10.0f, db / 20.0f);
        for (int x = 0; x < w; ++x)
        {
            const int bin = static_cast<int> ((static_cast<double> (x) / static_cast<double> (w)) * plotBins);
            if (bin < plotBins && mag[static_cast<std::size_t> (bin)] >= thresh)
            {
                const float hue = static_cast<float> (bin) / static_cast<float> (plotBins);
                row[static_cast<std::size_t> (x * 3)] = static_cast<unsigned char> (40 + hue * 200);
                row[static_cast<std::size_t> (x * 3 + 1)] = static_cast<unsigned char> (180 + hue * 60);
                row[static_cast<std::size_t> (x * 3 + 2)] = 240;
            }
        }
        out.write (reinterpret_cast<char*> (row.data()), static_cast<std::streamsize> (row.size()));
    }
    juce::ignoreUnused (label, sampleRate);
}

float aliasMetric (const std::vector<float>& mag, double sampleRate, float fundHz)
{
    const int n = static_cast<int> (mag.size());
    const float binHz = static_cast<float> (sampleRate) / static_cast<float> (n * 2);
    const int fundBin = static_cast<int> (fundHz / binHz);
    float fund = 0.0f;
    float alias = 0.0f;
    for (int k = std::max (1, fundBin - 2); k <= fundBin + 2; ++k)
        fund += mag[static_cast<std::size_t> (k)];
    for (int k = fundBin + 5; k < n; ++k)
        alias += mag[static_cast<std::size_t> (k)];
    return alias / (fund + 1.0e-9f);
}
} // namespace

int main (int argc, char** argv)
{
    const juce::File outDir = argc > 1 ? juce::File (argv[1]) : juce::File ("/opt/cursor/artifacts/junova-elite-audio");
    outDir.createDirectory();

    constexpr double sr = 44100.0;
    constexpr int c7 = 96;
    const float fundHz = 440.0f * std::pow (2.0f, (static_cast<float> (c7) - 69.0f) / 12.0f);

    junovax::dsp::SynthEngine engine;

    junovax::dsp::RuntimeParams naive {};
    naive.dcoSawOn = true;
    naive.dcoPulseOn = false;
    naive.dcoSubOn = false;
    naive.dcoNoise = 0.0f;
    naive.filterCutoff = 0.98f;
    naive.filterRes = 0.0f;
    naive.chorusMode = 0;
    naive.oversampleMode = 0;
    naive.measurementLegacyOsc = true;

    junovax::dsp::RuntimeParams elite = naive;
    elite.measurementLegacyOsc = false;
    elite.oversampleMode = 1;

    juce::AudioBuffer<float> bufNaive;
    juce::AudioBuffer<float> bufElite;
    renderEngine (engine, naive, c7, 1.5, sr, bufNaive);
    renderEngine (engine, elite, c7, 1.5, sr, bufElite);

    writeWav (outDir.getChildFile ("c7-saw-elite.wav"), bufElite, sr);

    std::vector<float> mono (static_cast<std::size_t> (bufElite.getNumSamples()));
    for (int i = 0; i < bufElite.getNumSamples(); ++i)
        mono[static_cast<std::size_t> (i)] = bufElite.getSample (0, i);

    std::vector<float> mag;
    fftMag (mono, mag);
    writePpmSpectrum (outDir.getChildFile ("c7-spectrum-after.ppm"), mag, sr, "elite");
    const float aliasElite = aliasMetric (mag, sr, fundHz);

    std::vector<float> magNaive;
    std::vector<float> monoNaive (static_cast<std::size_t> (bufNaive.getNumSamples()));
    for (int i = 0; i < bufNaive.getNumSamples(); ++i)
        monoNaive[static_cast<std::size_t> (i)] = bufNaive.getSample (0, i);
    fftMag (monoNaive, magNaive);
    writePpmSpectrum (outDir.getChildFile ("c7-spectrum-before.ppm"), magNaive, sr, "naive");
    const float aliasAfter = aliasMetric (magNaive, sr, fundHz);

    junovax::dsp::RuntimeParams sweep = elite;
    sweep.filterRes = 0.72f;
    sweep.filterCutoff = 0.05f;
    sweep.ampAttack = 0.002f;
    juce::AudioBuffer<float> bufSweep;
    renderEngine (engine, sweep, 60, 2.5, sr, bufSweep);
    writeWav (outDir.getChildFile ("filter-sweep.wav"), bufSweep, sr);

    junovax::dsp::RuntimeParams selfOsc = elite;
    selfOsc.filterRes = 0.98f;
    selfOsc.filterCutoff = 0.88f;
    selfOsc.dcoSawOn = false;
    selfOsc.dcoPulseOn = false;
    selfOsc.dcoSubOn = false;
    juce::AudioBuffer<float> bufSelf;
    renderEngine (engine, selfOsc, 60, 2.0, sr, bufSelf);
    writeWav (outDir.getChildFile ("filter-self-osc.wav"), bufSelf, sr);

    for (int mode = 1; mode <= 3; ++mode)
    {
        junovax::dsp::RuntimeParams ch = elite;
        ch.chorusMode = mode;
        ch.dcoSawOn = true;
        ch.dcoPulseOn = true;
        juce::AudioBuffer<float> bufCh;
        renderEngine (engine, ch, 57, 3.0, sr, bufCh);
        writeWav (outDir.getChildFile ("chorus-mode-" + juce::String (mode) + ".wav"), bufCh, sr);
    }

    const auto& presets = junovax::presets::getFactoryPresets();
    const int demoIds[] = { 0, 1, 8, 12, 16, 20, 24, 32 };
    for (int id : demoIds)
    {
        if (id >= static_cast<int> (presets.size()))
            continue;
        const auto& preset = presets[static_cast<std::size_t> (id)];
        junovax::dsp::RuntimeParams p {};
        const auto& pp = preset.params;
        p.masterGainDb = pp.masterGain;
        p.filterCutoff = pp.filterCutoff;
        p.filterRes = pp.filterRes;
        p.chorusMode = pp.chorusMode;
        p.dcoSawOn = pp.dcoSawOn;
        p.dcoPulseOn = pp.dcoPulseOn;
        p.drift = pp.drift;
        p.oversampleMode = 1;
        juce::AudioBuffer<float> bufP;
        renderEngine (engine, p, 60, 2.5, sr, bufP);
        const juce::String safeName (preset.name.data(), static_cast<int> (preset.name.size()));
        writeWav (outDir.getChildFile ("preset-" + juce::String (id) + "-" + safeName + ".wav"), bufP, sr);
    }

    engine.prepare (sr, 512);
    junovax::dsp::RuntimeParams cpuParams = elite;
    cpuParams.voiceMode = 0;
    engine.setParams (cpuParams);
    engine.reset();
    juce::AudioBuffer<float> cpuBuf (2, 512);
    juce::MidiBuffer midi;
    for (int n = 48; n < 72; ++n)
        midi.addEvent (juce::MidiMessage::noteOn (1, n, static_cast<juce::uint8> (95)), 0);

    const auto t0 = std::chrono::steady_clock::now();
    constexpr int iterations = 400;
    for (int i = 0; i < iterations; ++i)
    {
        cpuBuf.clear();
        juce::MidiBuffer mb;
        if (i == 0)
            mb = midi;
        engine.render (cpuBuf, mb);
    }
    const auto t1 = std::chrono::steady_clock::now();
    const double ms = std::chrono::duration<double, std::milli> (t1 - t0).count();
    const double blockMs = (512.0 / sr) * 1000.0;
    const double cpuPct = (ms / static_cast<double> (iterations)) / blockMs * 100.0;

    std::ofstream bench (outDir.getChildFile ("cpu-benchmark.txt").getFullPathName().toStdString());
    bench << "iterations=" << iterations << "\nblock=512 @ " << sr << " Hz\n";
    bench << "total_ms=" << ms << "\nper_block_ms=" << (ms / static_cast<double> (iterations)) << "\n";
    bench << "cpu_percent_one_core=" << cpuPct << "\n";
    bench << "alias_ratio_blep=" << aliasAfter << "\nalias_ratio_elite_os=" << aliasElite << "\n";

    std::cout << "JunovaEliteProof wrote " << outDir.getFullPathName() << "\n";
    return EXIT_SUCCESS;
}