# Junova-X — JUCE VST3 + CLAP + Standalone (WO-2026-001 / 002 skeleton)

juce_add_plugin (JunovaX
    COMPANY_NAME "SmFr"
    BUNDLE_ID com.smfr.junovax
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE SmFr
    PLUGIN_CODE JnvX
    FORMATS VST3 Standalone
    PRODUCT_NAME "Junova-X"
)

target_sources (JunovaX
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/UI/DiagPanel.cpp
        Source/UI/Celestial/CelestialMainPanel.cpp
        Source/UI/Celestial/OscMonitorComponent.cpp
        Source/DSP/SynthEngine.cpp
        Source/DSP/DiagTone.cpp
        Source/DSP/Arpeggiator.cpp
        Source/DSP/BbdChorus.cpp
        Source/Presets/FactoryPresets.cpp
)

add_executable (JunovaXTests
    Tests/DSP/ArpeggiatorTests.cpp
    Source/DSP/Arpeggiator.cpp
)

target_include_directories (JunovaXTests PRIVATE Source)
target_compile_features (JunovaXTests PRIVATE cxx_std_20)
target_compile_definitions (JunovaXTests PRIVATE JUCE_USE_CURL=0 JUCE_WEB_BROWSER=0)
target_link_libraries (JunovaXTests
    PRIVATE
        juce::juce_core
        juce::juce_audio_basics
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

add_test (NAME JunovaXArpeggiator COMMAND JunovaXTests)

target_compile_definitions (JunovaX
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries (JunovaX
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

target_include_directories (JunovaX PRIVATE Source)

juce_generate_juce_header (JunovaX)

clap_juce_extensions_plugin (TARGET JunovaX
    CLAP_ID "com.smfr.junovax"
    CLAP_FEATURES instrument synthesizer stereo
)
