#include "DSP/SynthEngine.h"
#include "DSP/ZdfMoogLadder.h"
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

float midiNoteToHz (int note) noexcept
{
    return 440.0f * std::pow (2.0f, (static_cast<float> (note) - 69.0f) / 12.0f);
}

int samplesForIntegerCycles (double sampleRate, float hz, int minCycles = 96) noexcept
{
    const double period = static_cast<double> (sampleRate) / static_cast<double> (hz);
    const int cycles = juce::jmax (minCycles, static_cast<int> (std::ceil (2.0 * sampleRate / period)));
    return static_cast<int> (std::round (static_cast<double> (cycles) * period));
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

    const float hz = midiNoteToHz (midiNote);
    int totalSamples = static_cast<int> (seconds * sampleRate);
    if (params.dcoSawOn && ! params.dcoPulseOn && ! params.dcoSubOn)
        totalSamples = samplesForIntegerCycles (sampleRate, hz, 128);

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
}

junovax::dsp::RuntimeParams dryC7SawBase() noexcept
{
    junovax::dsp::RuntimeParams p {};
    p.dcoSawOn = true;
    p.dcoPulseOn = false;
    p.dcoSubOn = false;
    p.dcoNoise = 0.0f;
    p.dcoLfoMod = 0.0f;
    p.pwmSourceLfo = false;
    p.drift = 0.0f;
    p.chorusMode = 0;
    p.hpfMode = 0;
    p.filterCutoff = 1.0f;
    p.filterRes = 0.0f;
    p.vcfEnv = 0.0f;
    p.vcfLfo = 0.0f;
    p.vcfDrive = 0.0f;
    p.dcoOctave = 2;
    p.ampAttack = 0.002f;
    p.ampSustain = 1.0f;
    p.filtSustain = 1.0f;
    return p;
}

float peakDbfs (const juce::AudioBuffer<float>& buf) noexcept
{
    float peak = 0.0f;
    for (int i = 0; i < buf.getNumSamples(); ++i)
        peak = juce::jmax (peak, std::abs (buf.getSample (0, i)));
    return 20.0f * std::log10 (peak + 1.0e-9f);
}

float estimatePitchHz (const juce::AudioBuffer<float>& buf, double sampleRate) noexcept
{
    const int n = buf.getNumSamples();
    const int tail = std::min (n, static_cast<int> (sampleRate * 0.5));
    const int offset = n - tail;
    int bestLag = 0;
    float bestCorr = -1.0f;
    const int minLag = static_cast<int> (sampleRate / 4000.0);
    const int maxLag = static_cast<int> (sampleRate / 40.0);
    for (int lag = minLag; lag < maxLag; ++lag)
    {
        double corr = 0.0;
        for (int i = 0; i < tail - lag; ++i)
            corr += static_cast<double> (buf.getSample (0, offset + i)) * buf.getSample (0, offset + i + lag);
        if (corr > bestCorr)
        {
            bestCorr = static_cast<float> (corr);
            bestLag = lag;
        }
    }
    return bestLag > 0 ? static_cast<float> (sampleRate) / static_cast<float> (bestLag) : 0.0f;
}

float normToCutoffHz (float norm, double sampleRate) noexcept
{
    const float minHz = 40.0f;
    const float maxHz = static_cast<float> (sampleRate * 0.45);
    return minHz * std::pow (maxHz / minHz, juce::jlimit (0.0f, 1.0f, norm));
}

int runPlotScript (const juce::File& outDir, float expectedHz)
{
    const auto script = juce::File::getCurrentWorkingDirectory().getChildFile ("Junova-X/tools/plot_elite_spectrum.py");
    juce::File scriptPath = script;
    if (! scriptPath.existsAsFile())
        scriptPath = juce::File ("/agent/repos/Instruments/Junova-X/tools/plot_elite_spectrum.py");

    const juce::String cmd = "python3 \"" + scriptPath.getFullPathName() + "\" \"" + outDir.getFullPathName()
                           + "\" " + juce::String (expectedHz, 2);
    return std::system (cmd.toRawUTF8());
}
} // namespace

int main (int argc, char** argv)
{
    const juce::File outDir = argc > 1 ? juce::File (argv[1]) : juce::File ("/opt/cursor/artifacts/junova-elite-audio");
    outDir.createDirectory();

    {
        junovax::dsp::ZdfMoogLadder bench;
        bench.prepare (44100.0);
        const float fc = normToCutoffHz (0.62f, 44100.0);
        bench.setTargets (fc, 1.0f, 1.05f);
        float p = 0.0f;
        juce::Random rnd (0xA11CEu);
        for (int i = 0; i < 44100 * 3; ++i)
        {
            const float exc = 3.0e-7f * (rnd.nextFloat() * 2.0f - 1.0f);
            const float y = bench.process (0.0f, exc);
            if (i > 44100)
                p = juce::jmax (p, std::abs (y));
        }
        std::cout << "zdf_ladder_isolated_peak=" << p << " (~" << (20.0f * std::log10 (p + 1e-9f)) << " dBFS) fc~" << fc << "\n";
    }

    constexpr double sr = 44100.0;
    constexpr int c7 = 96;
    const float fundHz = midiNoteToHz (c7);

    junovax::dsp::SynthEngine engine;

    auto naive = dryC7SawBase();
    naive.measurementLegacyOsc = true;
    naive.measurementBypassFilter = true;
    naive.oversampleMode = 0;

    auto eliteOsc = dryC7SawBase();
    eliteOsc.measurementLegacyOsc = false;
    eliteOsc.measurementBypassFilter = true;
    eliteOsc.oversampleMode = 0;

    juce::AudioBuffer<float> bufNaive;
    juce::AudioBuffer<float> bufElite;
    renderEngine (engine, naive, c7, 2.0, sr, bufNaive);
    renderEngine (engine, eliteOsc, c7, 2.0, sr, bufElite);

    writeWav (outDir.getChildFile ("c7-saw-before.wav"), bufNaive, sr);
    writeWav (outDir.getChildFile ("c7-saw-elite.wav"), bufElite, sr);

    auto elite = dryC7SawBase();
    elite.oversampleMode = 2;
    junovax::dsp::RuntimeParams sweep = elite;
    sweep.filterRes = 0.72f;
    sweep.filterCutoff = 0.05f;
    sweep.ampAttack = 0.002f;
    juce::AudioBuffer<float> bufSweep;
    renderEngine (engine, sweep, 60, 2.5, sr, bufSweep);
    writeWav (outDir.getChildFile ("filter-sweep.wav"), bufSweep, sr);

    junovax::dsp::RuntimeParams selfOsc = dryC7SawBase();
    selfOsc.dcoSawOn = false;
    selfOsc.dcoPulseOn = false;
    selfOsc.dcoSubOn = false;
    selfOsc.filterRes = 1.0f;
    selfOsc.filterCutoff = 0.62f;
    selfOsc.vcfEnv = 0.0f;
    selfOsc.vcfKey = 0.0f;
    selfOsc.vcfLfo = 0.0f;
    selfOsc.ampAttack = 0.001f;
    selfOsc.ampSustain = 1.0f;
    selfOsc.filtAttack = 0.001f;
    selfOsc.filtSustain = 1.0f;
    selfOsc.oversampleMode = 2;
    juce::AudioBuffer<float> bufSelf;
    renderEngine (engine, selfOsc, 72, 3.0, sr, bufSelf);
    writeWav (outDir.getChildFile ("filter-self-osc.wav"), bufSelf, sr);

    const float selfPeakDb = peakDbfs (bufSelf);
    const float selfPitch = estimatePitchHz (bufSelf, sr);
    const float selfPitchFft = [&bufSelf, sr]() -> float
    {
        const int n = juce::jmin (bufSelf.getNumSamples(), static_cast<int> (sr));
        const int offset = bufSelf.getNumSamples() - n;
        float maxMag = 0.0f;
        int maxBin = 0;
        std::vector<float> window (static_cast<std::size_t> (n), 0.0f);
        for (int i = 0; i < n; ++i)
            window[static_cast<std::size_t> (i)] = 0.5f - 0.5f * std::cos (juce::MathConstants<float>::twoPi * static_cast<float> (i)
                                                                               / static_cast<float> (n - 1));
        for (int k = 20; k < n / 4; ++k)
        {
            float re = 0.0f;
            float im = 0.0f;
            const float omega = juce::MathConstants<float>::twoPi * static_cast<float> (k) / static_cast<float> (n);
            for (int i = 0; i < n; ++i)
            {
                const float s = bufSelf.getSample (0, offset + i) * window[static_cast<std::size_t> (i)];
                re += s * std::cos (omega * static_cast<float> (i));
                im -= s * std::sin (omega * static_cast<float> (i));
            }
            const float mag = re * re + im * im;
            if (mag > maxMag)
            {
                maxMag = mag;
                maxBin = k;
            }
        }
        return static_cast<float> (maxBin) * static_cast<float> (sr) / static_cast<float> (n);
    }();
    const float selfCutoffHz = normToCutoffHz (selfOsc.filterCutoff, sr);

    for (int mode = 1; mode <= 3; ++mode)
    {
        junovax::dsp::RuntimeParams ch = elite;
        ch.chorusMode = mode;
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
        junovax::dsp::RuntimeParams p = dryC7SawBase();
        const auto& pp = preset.params;
        p.masterGainDb = pp.masterGain;
        p.filterCutoff = pp.filterCutoff;
        p.filterRes = pp.filterRes;
        p.chorusMode = pp.chorusMode;
        p.dcoSawOn = pp.dcoSawOn;
        p.dcoPulseOn = pp.dcoPulseOn;
        p.drift = pp.drift;
        p.dcoOctave = pp.dcoOctave;
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
    bench << "c7_expected_hz=" << fundHz << "\n";
    bench << "self_osc_peak_dbfs=" << selfPeakDb << "\n";
    bench << "self_osc_pitch_hz=" << selfPitch << "\n";
    bench << "self_osc_fft_peak_hz=" << selfPitchFft << "\n";
    bench << "self_osc_cutoff_hz=" << selfCutoffHz << "\n";
    bench << "iterations=" << iterations << "\nblock=512 @ " << sr << " Hz\n";
    bench << "total_ms=" << ms << "\nper_block_ms=" << (ms / static_cast<double> (iterations)) << "\n";
    bench << "cpu_percent_one_core=" << cpuPct << "\n";

    runPlotScript (outDir, fundHz);

    std::cout << "JunovaEliteProof wrote " << outDir.getFullPathName() << "\n";
    std::cout << "self_osc_peak_dbfs=" << selfPeakDb << " pitch_hz=" << selfPitch << " fft_peak_hz=" << selfPitchFft
              << " cutoff_hz=" << selfCutoffHz << '\n';
    return EXIT_SUCCESS;
}