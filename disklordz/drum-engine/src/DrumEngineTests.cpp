#include "ClapVoice.h"
#include "HiHatVoice.h"
#include "Kick808Voice.h"
#include "LicenseManager.h"
#include "MidiMaps.h"
#include "SnareVoice.h"

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

#include <cmath>
#include <cstring>
#include <iostream>
#include <string>

namespace {

int gFailures = 0;

void check(bool condition, const char *text) {
    if (!condition) {
        std::cerr << "FAIL " << text << '\n';
        ++gFailures;
    }
}

template <typename Voice> juce::AudioBuffer<float> render(const typename Voice::Params &params, double seconds) {
    Voice voice;
    constexpr int block = 256;
    voice.prepare({48000.0, static_cast<juce::uint32>(block), 1});
    const int total = static_cast<int>(seconds * 48000.0);
    juce::AudioBuffer<float> buffer(1, total);
    buffer.clear();
    voice.trigger(params, 1.0f);
    for (int pos = 0; pos < total; pos += block)
        voice.process(buffer, pos, std::min(block, total - pos));
    return buffer;
}

bool finiteAndBounded(const juce::AudioBuffer<float> &buffer) {
    const auto *samples = buffer.getReadPointer(0);
    for (int i = 0; i < buffer.getNumSamples(); ++i)
        if (!std::isfinite(samples[i]) || std::abs(samples[i]) > 1.05f)
            return false;
    return true;
}

float peakOf(const juce::AudioBuffer<float> &buffer, int start, int count) {
    float peak = 0.0f;
    const auto *samples = buffer.getReadPointer(0);
    const int end = std::min(buffer.getNumSamples(), start + count);
    for (int i = start; i < end; ++i)
        peak = std::max(peak, std::abs(samples[i]));
    return peak;
}

bool sameSamples(const juce::AudioBuffer<float> &a, const juce::AudioBuffer<float> &b) {
    if (a.getNumSamples() != b.getNumSamples())
        return false;
    const auto *left = a.getReadPointer(0);
    const auto *right = b.getReadPointer(0);
    const auto bytes = sizeof(float) * static_cast<size_t>(a.getNumSamples());
    return std::memcmp(left, right, bytes) == 0;
}

void testKick() {
    dlz::drum::Kick808Voice::Params params;
    params.ampDecaySec = 0.12f;
    params.pitchDecaySec = 0.03f;
    const auto first = render<dlz::drum::Kick808Voice>(params, 2.0);
    const auto second = render<dlz::drum::Kick808Voice>(params, 2.0);
    check(sameSamples(first, second), "kick phase reset is deterministic");
    check(finiteAndBounded(first), "kick stays finite and inside [-1.05, 1.05]");
    check(peakOf(first, 0, 4000) > 0.05f, "kick has an audible attack");
    check(peakOf(first, static_cast<int>(1.6 * 48000.0), 8000) < 1.0e-3f, "kick tail settles");

    dlz::drum::Kick808Voice voice;
    voice.prepare({48000.0, 128, 1});
    const int capacity = voice.scratchCapacity();
    voice.trigger(params, 1.0f);
    juce::AudioBuffer<float> block(1, 128);
    block.clear();
    voice.process(block, 0, 128);
    check(voice.scratchCapacity() == capacity, "kick scratch buffer is not resized in process");
    check(voice.oversamplingFactor() == 4, "JUCE factor 2 is 4x oversampling");
}

void testOtherVoices() {
    check(finiteAndBounded(render<dlz::drum::SnareVoice>({}, 0.8)), "snare finite");
    check(finiteAndBounded(render<dlz::drum::ClapVoice>({}, 0.8)), "clap finite");
    dlz::drum::HiHatVoice::Params hat;
    check(finiteAndBounded(render<dlz::drum::HiHatVoice>(hat, 0.4)), "hat finite");
    const auto a = render<dlz::drum::SnareVoice>({}, 0.4);
    const auto b = render<dlz::drum::SnareVoice>({}, 0.4);
    check(sameSamples(a, b), "snare noise seed resets on trigger");
    check(peakOf(a, 0, 2000) > 0.02f, "snare has an attack");
}

void testLicense() {
    const auto grace = dlz::evaluateLicense(true, false, 2'000'000, 1'000'000, 1'500'000);
    const auto expired = dlz::evaluateLicense(true, false, 5'000'000, 1'000'000, 1'500'000);
    const auto server = dlz::evaluateLicense(false, true, 0, 0, 0);
    const auto missing = dlz::evaluateLicense(false, false, 10, 1, 100);
    check(grace.allowed && std::string(grace.reason) == "offline-grace", "offline grace allows a cached token");
    check(!expired.allowed, "expired grace blocks playback");
    check(server.allowed, "server valid allows playback");
    check(!missing.allowed, "missing token fails closed");

    const auto dir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                         .getChildFile("dlz-license-test-" + juce::String(juce::Random::getSystemRandom().nextInt()));
    dir.createDirectory();
    const auto state = dir.getChildFile("license.settings");
    {
        juce::PropertiesFile::Options options;
        options.storageFormat = juce::PropertiesFile::storeAsXML;
        juce::PropertiesFile props(state, options);
        props.setValue("token", "studio-token");
        props.setValue("lastOkMs", juce::String(juce::Time::currentTimeMillis()));
        props.saveIfNeeded();
    }

    dlz::LicenseManager manager(state);
    manager.bindAudioThread();
    check(!manager.requestActivation(), "activation is refused on the audio thread");
    check(!manager.allowsProcessing(), "audio thread refusal does not flip the gate");

    dlz::LicenseManager worker(state);
    check(worker.requestActivation(), "activation starts off the audio thread");
    const auto deadline = juce::Time::getMillisecondCounter() + 2000;
    while (!worker.allowsProcessing() && juce::Time::getMillisecondCounter() < deadline)
        juce::Thread::sleep(10);
    check(worker.allowsProcessing(), "cached token within grace opens the gate");
    dir.deleteRecursively();
}

void testMidiMap() {
    check(dlz::midi::kDrumVoices[0].gmNote == 36, "GM kick is note 36");
    check(dlz::midi::kDrumVoices[1].gmNote == 38, "GM snare is note 38");
    check(dlz::midi::kClosedHat == 42, "GM closed hat is note 42");
    check(dlz::midi::findVoice("808") != nullptr, "808 token is registered");
    check(dlz::midi::findVoice("nope") == nullptr, "unknown token is absent");
}

} // namespace

int main() {
    testKick();
    testOtherVoices();
    testLicense();
    testMidiMap();
    if (gFailures != 0) {
        std::cerr << gFailures << " failure(s)\n";
        return 1;
    }
    std::cout << "DrumEngineTests passed\n";
    return 0;
}
