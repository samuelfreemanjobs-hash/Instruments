# Huckaby Pattern Engine — polyrhythmic step sequencer + hot-swappable sample banks (JUCE)
# Built as a subdirectory of the Instruments monorepo root CMake project.

juce_add_plugin(HuckabyPatternEngine
    COMPANY_NAME "Instruments"
    BUNDLE_ID com.instruments.huckabypatternengine
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Inst
    PLUGIN_CODE Hkby
    FORMATS VST3 Standalone
    PRODUCT_NAME "Huckaby Pattern Engine"
)

target_sources(HuckabyPatternEngine
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
)

target_compile_definitions(HuckabyPatternEngine
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(HuckabyPatternEngine
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_audio_formats
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

target_include_directories(HuckabyPatternEngine
    PUBLIC
        Source
)

juce_generate_juce_header(HuckabyPatternEngine)

add_executable(HuckabyPatternEngine_Smoke
    Tests/SmokeProcessor.cpp
)

target_link_libraries(HuckabyPatternEngine_Smoke
    PRIVATE
        HuckabyPatternEngine
        juce::juce_gui_basics
)

target_include_directories(HuckabyPatternEngine_Smoke
    PRIVATE
        Source
        "${CMAKE_CURRENT_BINARY_DIR}/HuckabyPatternEngine_artefacts/JuceLibraryCode"
)

add_dependencies(HuckabyPatternEngine_Smoke HuckabyPatternEngine)

add_test(NAME HuckabyPatternEngine_Smoke COMMAND HuckabyPatternEngine_Smoke)
