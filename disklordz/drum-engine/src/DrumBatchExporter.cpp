#include "ClapVoice.h"
#include "HiHatVoice.h"
#include "Kick808Voice.h"
#include "MidiMaps.h"
#include "SnareVoice.h"

#include <juce_audio_formats/juce_audio_formats.h>

#include <cmath>
#include <iostream>
#include <memory>
#include <string>

namespace {

constexpr double kSampleRate = 48000.0;
constexpr int kBitDepth = 24;
constexpr int kBlockSize = 512;
constexpr float kPeakTarget = 0.966050878f; // -0.3 dBFS
constexpr float kRoots[] = {36.71f, 41.20f, 43.65f, 49.00f, 55.00f};
constexpr const char *kNoteNames[] = {"D1", "E1", "F1", "G1", "A1"};

struct Options {
    juce::String out{"Exported_Drum_Kit"};
    int count = 5;
    juce::int64 seed = 42;
    juce::String voice{"all"};
    juce::String brand{"DLZ"};
    juce::String kit{"808Lab"};
};

int finishOneShot(juce::AudioBuffer<float> &buffer) {
    const int available = buffer.getNumSamples();
    auto *samples = buffer.getWritePointer(0);
    double sum = 0.0;
    for (int i = 0; i < available; ++i)
        sum += samples[i];
    const float mean = static_cast<float>(sum / static_cast<double>(juce::jmax(1, available)));
    for (int i = 0; i < available; ++i)
        samples[i] -= mean;

    int last = 0;
    for (int i = 0; i < available; ++i)
        if (std::abs(samples[i]) > 1.0e-4f)
            last = i;

    int length = juce::jmin(available, last + static_cast<int>(0.02 * kSampleRate) + 1);
    length = juce::jmax(length, static_cast<int>(0.05 * kSampleRate));

    const int fade = juce::jmin(64, length / 4);
    for (int i = 0; i < fade; ++i) {
        const float gain = static_cast<float>(i) / static_cast<float>(fade);
        samples[i] *= gain;
        samples[length - 1 - i] *= gain;
    }

    float peak = 0.0f;
    for (int i = 0; i < length; ++i)
        peak = std::max(peak, std::abs(samples[i]));
    if (peak > 1.0e-8f) {
        const float gain = kPeakTarget / peak;
        for (int i = 0; i < length; ++i)
            samples[i] *= gain;
    }
    return length;
}

bool writeWav(const juce::File &file, const juce::AudioBuffer<float> &buffer, int length) {
    file.deleteFile();
    auto stream = std::unique_ptr<juce::OutputStream>(file.createOutputStream());
    if (stream == nullptr)
        return false;

    juce::WavAudioFormat wav;
    auto writer =
        wav.createWriterFor(stream, juce::AudioFormatWriterOptions{}
                                        .withSampleRate(kSampleRate)
                                        .withNumChannels(1)
                                        .withBitsPerSample(kBitDepth)
                                        .withSampleFormat(juce::AudioFormatWriterOptions::SampleFormat::integral));
    if (writer == nullptr)
        return false;
    return writer->writeFromAudioSampleBuffer(buffer, 0, length);
}

template <typename Voice, typename ParamFn>
int renderVoice(Voice &voice, ParamFn &&makeParams, const Options &options, const char *token, juce::Random &rng,
                int &written) {
    voice.prepare({kSampleRate, static_cast<juce::uint32>(kBlockSize), 1});
    const int total = static_cast<int>(kSampleRate * 2.5);
    juce::AudioBuffer<float> buffer(1, total);

    for (int index = 0; index < options.count; ++index) {
        buffer.clear();
        voice.trigger(makeParams(index, rng), 1.0f);
        for (int pos = 0; pos < total; pos += kBlockSize)
            voice.process(buffer, pos, std::min(kBlockSize, total - pos));

        const int length = finishOneShot(buffer);
        const int noteIndex = index % 5;
        const auto fileName = juce::String::formatted("%s_%s_%s_%02d_%s.wav", options.brand.toRawUTF8(),
                                                      options.kit.toRawUTF8(), token, index + 1, kNoteNames[noteIndex]);
        const auto file = juce::File(options.out).getChildFile(fileName);
        if (!writeWav(file, buffer, length)) {
            std::cerr << "failed to write " << file.getFullPathName() << '\n';
            return 1;
        }
        ++written;
    }
    return 0;
}

Options parse(int argc, char **argv) {
    Options options;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        auto need = [&](const char *flag) -> const char * {
            if (arg != flag || i + 1 >= argc)
                return nullptr;
            return argv[++i];
        };
        const char *value = nullptr;
        if (arg == "--out")
            value = need("--out");
        else if (arg == "--count")
            value = need("--count");
        else if (arg == "--seed")
            value = need("--seed");
        else if (arg == "--voice")
            value = need("--voice");
        else if (arg == "--brand")
            value = need("--brand");
        else if (arg == "--kit")
            value = need("--kit");
        else {
            std::cerr << "Unknown argument: " << arg << '\n';
            continue;
        }
        if (value == nullptr)
            continue;
        if (arg == "--out")
            options.out = value;
        else if (arg == "--count")
            options.count = std::max(1, std::atoi(value));
        else if (arg == "--seed")
            options.seed = std::atoll(value);
        else if (arg == "--voice")
            options.voice = value;
        else if (arg == "--brand")
            options.brand = value;
        else if (arg == "--kit")
            options.kit = value;
    }
    return options;
}

} // namespace

int main(int argc, char **argv) {
    const Options options = parse(argc, argv);
    juce::File outDir(options.out);
    outDir.createDirectory();
    juce::Random rng(options.seed);
    int written = 0;
    const auto voice = options.voice.toLowerCase();
    const bool all = voice == "all";

    if (all || voice == "kick" || voice == "808") {
        dlz::drum::Kick808Voice kick;
        if (renderVoice(
                kick,
                [](int index, juce::Random &random) {
                    dlz::drum::Kick808Voice::Params params;
                    params.baseFreqHz = kRoots[index % 5];
                    params.pitchEnvHz = 90.0f + random.nextFloat() * 140.0f;
                    params.pitchDecaySec = 0.025f + random.nextFloat() * 0.055f;
                    params.ampDecaySec = 0.45f + random.nextFloat() * 0.7f;
                    params.drive = 1.2f + random.nextFloat() * 4.0f;
                    params.clickAmount = random.nextFloat() * 0.45f;
                    return params;
                },
                options, "808", rng, written) != 0)
            return 1;
    }

    if (all || voice == "snare") {
        dlz::drum::SnareVoice snare;
        if (renderVoice(
                snare,
                [](int, juce::Random &random) {
                    dlz::drum::SnareVoice::Params params;
                    params.toneHz = 150.0f + random.nextFloat() * 120.0f;
                    params.noiseMix = 0.3f + random.nextFloat() * 0.4f;
                    params.ampDecaySec = 0.16f + random.nextFloat() * 0.25f;
                    params.drive = 1.2f + random.nextFloat() * 2.5f;
                    return params;
                },
                options, "Snare", rng, written) != 0)
            return 1;
    }

    if (all || voice == "clap") {
        dlz::drum::ClapVoice clap;
        if (renderVoice(
                clap,
                [](int, juce::Random &random) {
                    dlz::drum::ClapVoice::Params params;
                    params.spacingSec = 0.008f + random.nextFloat() * 0.006f;
                    params.tailDecaySec = 0.12f + random.nextFloat() * 0.16f;
                    params.drive = 1.2f + random.nextFloat() * 2.0f;
                    return params;
                },
                options, "Clap", rng, written) != 0)
            return 1;
    }

    if (all || voice == "hat") {
        dlz::drum::HiHatVoice hat;
        if (renderVoice(
                hat,
                [](int index, juce::Random &random) {
                    dlz::drum::HiHatVoice::Params params;
                    params.openHat = (index % 4) == 3;
                    params.fundamentalHz = 280.0f + random.nextFloat() * 80.0f;
                    params.decaySec =
                        params.openHat ? 0.28f + random.nextFloat() * 0.2f : 0.03f + random.nextFloat() * 0.04f;
                    params.drive = 1.1f + random.nextFloat() * 1.5f;
                    return params;
                },
                options, "Hat", rng, written) != 0)
            return 1;
    }

    if (written == 0) {
        std::cerr << "No voices rendered. Use --voice kick|snare|clap|hat|all\n";
        return 2;
    }

    std::cout << "exported=" << written << " bitDepth=" << kBitDepth << " sampleRate=" << static_cast<int>(kSampleRate)
              << " dir=" << outDir.getFullPathName() << '\n';
    juce::ignoreUnused(dlz::midi::kDrumVoices);
    return 0;
}
