# MPC Renaissance custom host — JUCE standalone (Windows box / Linux custom engine)

juce_add_plugin (RenaissanceHost
    COMPANY_NAME "Instruments"
    BUNDLE_ID com.instruments.renaissancehost
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Inst
    PLUGIN_CODE RnHs
    FORMATS Standalone
    PRODUCT_NAME "Renaissance Host"
)

target_sources (RenaissanceHost
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/RenaissanceSurfaceMap.cpp
        Source/PadVoiceEngine.cpp
        Source/LcdBridge.cpp
)

target_compile_definitions (RenaissanceHost
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries (RenaissanceHost
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

target_include_directories (RenaissanceHost PRIVATE Source)

juce_generate_juce_header (RenaissanceHost)

add_executable (RenaissanceHostSurfaceMapTests
    Tests/SurfaceMapTests.cpp
    Source/RenaissanceSurfaceMap.cpp
)

target_include_directories (RenaissanceHostSurfaceMapTests PRIVATE Source)
target_compile_features (RenaissanceHostSurfaceMapTests PRIVATE cxx_std_20)

add_test (NAME RenaissanceHostSurfaceMap COMMAND RenaissanceHostSurfaceMapTests)
