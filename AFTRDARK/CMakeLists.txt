# AFTRDARK — built as a subdirectory of the Instruments monorepo root CMake project.

juce_add_plugin (AFTRDARK
    COMPANY_NAME "Instruments"
    BUNDLE_ID com.instruments.aftrdark
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Inst
    PLUGIN_CODE AftD
    FORMATS VST3 Standalone
    PRODUCT_NAME "AFTRDARK"
)

target_sources (AFTRDARK
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/Presets/FactoryPresets.cpp
)

target_compile_definitions (AFTRDARK
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries (AFTRDARK
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

target_include_directories (AFTRDARK PRIVATE Source)

juce_generate_juce_header (AFTRDARK)

add_executable (AftrdarkTests
    Tests/DSP/WavetableBankTests.cpp
)

target_include_directories (AftrdarkTests PRIVATE Source)
target_compile_features (AftrdarkTests PRIVATE cxx_std_20)

add_test (NAME AftrdarkWavetable COMMAND AftrdarkTests)
