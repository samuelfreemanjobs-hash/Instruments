---
name: disklordz-juce-coding
description: JUCE 8 plugin factory — CMake, APVTS, processor/editor, VST3/CLAP/standalone, state, buses, and Instruments monorepo conventions.
when_to_use: Implement or refactor JUCE AudioProcessor, plugin CMake targets, editors, parameters, presets, MIDI/SysEx, or host format wiring.
disable-model-invocation: false
context: fork
paths:
  - "CMakeLists.txt"
  - "Source/**"
  - "Wave9090/**"
  - "SP1200/**"
  - "docs/INTEGRATION.md"
  - "docs/ARCHITECTURE.md"
  - "MyFirstPlugin/**"
---

# Skill — JUCE plugin coding (Instruments factory)

## Repo pins (do not guess versions)

| Item | Value | Where |
|------|-------|--------|
| JUCE | **8.0.15** (`FetchContent` `GIT_TAG`) | Root `CMakeLists.txt` |
| C++ standard | **20** | `CMAKE_CXX_STANDARD 20` |
| CLAP | `clap-juce-extensions` pinned commit | Root `CMakeLists.txt` + [docs/INTEGRATION.md](../../../../../docs/INTEGRATION.md) |
| Linux CI compilers | **g++-12** / **gcc-12** | `AGENTS.md`, `build.yml` |

Read [docs/INTEGRATION.md](../../../../docs/INTEGRATION.md) before changing CLAP or format flags.

## Greenfield plugin checklist

1. **CMake** — `juce_add_plugin` with unique `PLUGIN_CODE` / `BUNDLE_ID`, `FORMATS VST3 Standalone` (add CLAP via `clap_juce_extensions_plugin` when shipping JD-style products).
2. **Processor** — `prepareToPlay` allocates/resizes; `releaseResources` drops heavy buffers; `processBlock` stays realtime-safe (see `disklordz-cpp-realtime` skill).
3. **APVTS** — static `createParameterLayout()`; member `AudioProcessorValueTreeState apvts_`; cache raw parameter pointers in `prepareToPlay` or `refreshCachedParameters()` if read per block.
4. **Editor** — `createEditor()` returns `AudioProcessorEditor`; use `AudioProcessorValueTreeState::SliderAttachment` (or custom) on **message thread** only.
5. **State** — `getStateInformation` / `setStateInformation` via APVTS `copyState()` / `replaceState()` unless you version a custom binary chunk (document migration).
6. **Docs** — add row to product `ARCHITECTURE.md` extension table.
7. **CI** — ensure target name appears in `vst-testing-ops` artefact stage or extend pipeline explicitly.

## Canonical processor shape (this monorepo)

Follow **`JDUpgradedAudioProcessor`** ([Source/PluginProcessor.h](../../../../Source/PluginProcessor.h)):

```text
prepareToPlay  → voice pool / DSP reset, sample rate, max block size
processBlock   → MIDI handle → render → master FX → clear extra outputs
APVTS          → createParameterLayout + applyPatchesFromParameters pattern
Programs       → factory patch library OR preset files (product-specific)
```

Wave9090 and SP1200 mirror the same split under their directories — read their `ARCHITECTURE.md` before copying patterns.

## APVTS rules

- Parameter IDs are **stable strings** — never rename without preset migration plan (R10).
- Prefer `AudioParameterFloat` with sensible ranges and `StringRef` IDs in one translation unit (`createParameterLayout`).
- For discrete choices use `AudioParameterChoice`; expose `getIndex()` on audio thread only if cached as atomic/int updated on message thread.
- **Smoothed values:** `juce::SmoothedValue<float>` or per-block ramp — avoid zipper noise on filter cutoff / gain.
- **Bypass:** if added, use `ProcessContext`-style dry/wet or JUCE DSP `DryWetMixer` at control boundaries — document latency.

## Threading contract

| Thread | Allowed |
|--------|---------|
| **Audio** (`processBlock`) | Read atomics, preallocated buffers, lock-free FIFOs filled on message thread |
| **Message** | APVTS gestures, file choosers, SysEx file import, `AsyncUpdater` |
| **UI** | Components, attachments, paint |

Never call `MessageManager::callAsync` from audio thread to “fix” UI. Queue atomic flags or `AbstractFifo` for meter data UI reads.

## Formats & hosting

- **VST3:** `JUCE_VST3_CAN_REPLACE_VST2=0` is set project-wide — keep it unless product requires VST2 bridge (we do not ship VST2).
- **CLAP:** call `clap_juce_extensions_plugin` **after** `target_link_libraries` for the JUCE target; CLAP ID should align with `BUNDLE_ID`.
- **Standalone:** same processor class; test with `JDUpgraded_Standalone` (or product equivalent) before trusting DAW-only behavior.
- **pluginval:** strictness 5 via `scripts/vst/run_pluginval.py` — required for release-quality claims.

## CMake snippets (copy/adapt)

```cmake
juce_add_plugin(MyProduct
    COMPANY_NAME "Instruments"
    BUNDLE_ID com.instruments.myproduct
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    FORMATS VST3 Standalone
    PRODUCT_NAME "My Product")

target_compile_definitions(MyProduct
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(MyProduct
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
```

## Common defects (fix these first)

- Denormals not scoped — use `juce::ScopedNoDenormals` at top of `processBlock`.
- Bus mismatch — implement `isBusesLayoutSupported` explicitly for mono/stereo expectations.
- Editor touching processor state without APVTS — race with audio thread.
- `copyPluginAfterBuild TRUE` in CI — leave **FALSE** in repo (local dev may enable).
- Heavy work in `createParameterLayout` — keep layout static; no I/O.

## Verify

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target MyProduct_VST3 MyProduct_Standalone
python3 vst-testing-ops/run_business.py --profile dsp-only
```

## Related skills

- **`disklordz-cpp-realtime`** — audio-thread C++ and SIMD
- **`disklordz-dsp-vst-architect`** — coordinator agent + factory loop
