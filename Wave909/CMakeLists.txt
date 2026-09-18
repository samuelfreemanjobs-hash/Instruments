# WAVE-909 — built as a subdirectory of the Instruments monorepo root CMake project.

juce_add_plugin (Wave909
    COMPANY_NAME "Instruments"
    BUNDLE_ID com.instruments.wave909
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Inst
    PLUGIN_CODE Wv09
    FORMATS VST3 Standalone
    PRODUCT_NAME "WAVE-909"
)

target_sources (Wave909
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/Presets/FactoryPresets.cpp
)

target_compile_definitions (Wave909
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries (Wave909
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

target_include_directories (Wave909 PRIVATE Source)

juce_generate_juce_header (Wave909)

add_executable (Wave909Tests
    Tests/DSP/WavetableBankTests.cpp
)

target_include_directories (Wave909Tests PRIVATE Source)
target_compile_features (Wave909Tests PRIVATE cxx_std_20)

add_test (NAME Wave909Wavetable COMMAND Wave909Tests)
