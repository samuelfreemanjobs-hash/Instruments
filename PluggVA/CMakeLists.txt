# Plugg VA — virtual-analog trap synth (subdirectory of Instruments root CMake).

juce_add_plugin (PluggVA
    COMPANY_NAME "Instruments"
    BUNDLE_ID com.instruments.pluggva
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Inst
    PLUGIN_CODE PlVA
    FORMATS VST3 Standalone
    PRODUCT_NAME "Plugg VA"
)

target_sources (PluggVA
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/Presets/FactoryPresets.cpp
)

target_compile_definitions (PluggVA
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries (PluggVA
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

target_include_directories (PluggVA PRIVATE Source)

juce_generate_juce_header (PluggVA)

add_executable (PluggVATests
    Tests/DSP/OscillatorTests.cpp
    Source/Presets/FactoryPresets.cpp
)

target_include_directories (PluggVATests PRIVATE Source)
target_compile_features (PluggVATests PRIVATE cxx_std_20)

add_test (NAME PluggVAOscillator COMMAND PluggVATests)
