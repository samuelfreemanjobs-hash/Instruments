juce_add_plugin(Rev2Trap
    COMPANY_NAME "Instruments"
    BUNDLE_ID com.instruments.rev2trap
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Inst
    PLUGIN_CODE Rv2T
    FORMATS VST3 CLAP Standalone
    PRODUCT_NAME "Rev2 Trap"
)

target_sources(Rev2Trap
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/Presets/FactoryPresets.cpp
        Source/Presets/Jz400Presets.cpp
)

target_compile_definitions(Rev2Trap
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(Rev2Trap
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

target_include_directories(Rev2Trap PRIVATE Source)
juce_generate_juce_header(Rev2Trap)

clap_juce_extensions_plugin(TARGET Rev2Trap
    CLAP_ID "com.instruments.rev2trap"
    CLAP_FEATURES instrument synthesizer stereo
)

add_executable(Rev2TrapOfflineRender tools/Rev2TrapOfflineRender.cpp)
target_link_libraries(Rev2TrapOfflineRender
    PRIVATE
        Rev2Trap
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_utils
)
target_include_directories(Rev2TrapOfflineRender PRIVATE Source)
target_compile_definitions(Rev2TrapOfflineRender PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)

add_executable(Rev2TrapTests Tests/ProcessorSmokeTest.cpp)
target_link_libraries(Rev2TrapTests PRIVATE Rev2Trap juce::juce_audio_processors)
target_include_directories(Rev2TrapTests PRIVATE Source)
add_test(NAME Rev2TrapSmoke COMMAND Rev2TrapTests)
