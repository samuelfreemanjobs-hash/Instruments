#pragma once

#include <juce_data_structures/juce_data_structures.h>

#include <atomic>

namespace dlz {

struct LicenseDecision {
    bool allowed = false;
    const char *reason = "missing";
};

// Pure decision. Network I/O and PropertiesFile reads stay off this function
// so the audio thread can trust a single atomic written by the license thread.
inline LicenseDecision evaluateLicense(bool hasToken, bool serverValid, juce::int64 nowMs, juce::int64 lastOkMs,
                                       juce::int64 graceMs) noexcept {
    if (serverValid)
        return {true, "server"};
    const bool clockOk = lastOkMs > 0 && nowMs >= lastOkMs;
    if (hasToken && clockOk && graceMs > 0 && (nowMs - lastOkMs) <= graceMs)
        return {true, "offline-grace"};
    if (!hasToken)
        return {false, "no-token"};
    return {false, "grace-expired"};
}

// Caches an activation token in a PropertiesFile and refreshes it on a
// background thread. processBlock may only call allowsProcessing().
class LicenseManager : private juce::Thread {
  public:
    static constexpr juce::int64 defaultGraceMs = 14LL * 24LL * 60LL * 60LL * 1000LL;

    LicenseManager(juce::File stateFileIn, juce::String endpointIn = {})
        : juce::Thread("dlz-license"), stateFile(std::move(stateFileIn)), endpoint(std::move(endpointIn)) {}

    ~LicenseManager() override { stopThread(2000); }

    void bindAudioThread() noexcept { audioThreadId.store(juce::Thread::getCurrentThreadId()); }

    // Audio thread: one atomic load. No file, socket, or lock.
    bool allowsProcessing() const noexcept { return allowed.load(std::memory_order_acquire); }

    // Returns false when called on the bound audio thread so a mistake cannot
    // start a network read inside processBlock. Message-thread callers should
    // still prefer this over a synchronous URL read on the message thread.
    bool requestActivation() {
        if (juce::Thread::getCurrentThreadId() == audioThreadId.load())
            return false;
        if (checkRunning.exchange(true))
            return true;
        startThread();
        return true;
    }

    LicenseDecision evaluateStored(juce::int64 nowMs) const { return decideFromFile(nowMs, false); }

  private:
    void run() override {
        const auto decision = decideFromFile(juce::Time::currentTimeMillis(), true);
        allowed.store(decision.allowed, std::memory_order_release);
        checkRunning.store(false);
    }

    LicenseDecision decideFromFile(juce::int64 nowMs, bool allowNetwork) const {
        bool hasToken = false;
        juce::int64 lastOkMs = 0;
        if (stateFile.existsAsFile()) {
            const juce::PropertiesFile::Options options = makeOptions();
            juce::PropertiesFile props(stateFile, options);
            hasToken = props.getValue("token").isNotEmpty();
            lastOkMs = props.getValue("lastOkMs").getLargeIntValue();
        }

        bool serverValid = false;
        if (allowNetwork && endpoint.isNotEmpty() && !threadShouldExit()) {
            const auto body = juce::URL(endpoint).readEntireTextStream();
            serverValid = body.containsIgnoreCase("\"valid\":true") || body.containsIgnoreCase("valid=true");
            if (serverValid)
                writeSuccess(nowMs);
        }

        return evaluateLicense(hasToken, serverValid, nowMs, lastOkMs, defaultGraceMs);
    }

    void writeSuccess(juce::int64 nowMs) const {
        stateFile.getParentDirectory().createDirectory();
        const juce::PropertiesFile::Options options = makeOptions();
        juce::PropertiesFile props(stateFile, options);
        if (props.getValue("token").isEmpty())
            props.setValue("token", "activated");
        props.setValue("lastOkMs", juce::String(nowMs));
        props.saveIfNeeded();
    }

    static juce::PropertiesFile::Options makeOptions() {
        juce::PropertiesFile::Options options;
        options.storageFormat = juce::PropertiesFile::storeAsXML;
        options.commonToAllUsers = false;
        return options;
    }

    juce::File stateFile;
    juce::String endpoint;
    std::atomic<bool> allowed{false};
    std::atomic<bool> checkRunning{false};
    std::atomic<juce::Thread::ThreadID> audioThreadId{nullptr};
};

} // namespace dlz
