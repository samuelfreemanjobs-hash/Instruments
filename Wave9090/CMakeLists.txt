# WAVE-9090 — built as a subdirectory of the Instruments monorepo root CMake project.

juce_add_plugin (Wave9090
    COMPANY_NAME "Instruments"
    BUNDLE_ID com.instruments.wave9090
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Inst
    PLUGIN_CODE Wv90
    FORMATS VST3 Standalone
    PRODUCT_NAME "WAVE-9090"
)

target_sources (Wave9090
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/Presets/FactoryPresets.cpp
)

target_compile_definitions (Wave9090
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries (Wave9090
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

target_include_directories (Wave9090 PRIVATE Source)

juce_generate_juce_header (Wave9090)

add_executable (Wave9090Tests
    Tests/DSP/WavetableBankTests.cpp
)

target_include_directories (Wave9090Tests PRIVATE Source)
target_compile_features (Wave9090Tests PRIVATE cxx_std_20)

add_test (NAME Wave9090Wavetable COMMAND Wave9090Tests)
