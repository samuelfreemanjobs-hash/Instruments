# WAVE-9090 — built as a subdirectory of the Instruments monorepo root CMake project.

set(WAVE9090_SKIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Assets/skin")

juce_add_binary_data (Wave9090SkinData
    SOURCES
        "${WAVE9090_SKIN_DIR}/panel_bg.png"
        "${WAVE9090_SKIN_DIR}/knob_dial.png"
        "${WAVE9090_SKIN_DIR}/header_strip.png"
)

juce_add_plugin (Wave9090
    COMPANY_NAME "Instruments"
    BUNDLE_ID com.instruments.wave9090
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
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
        Source/UI/Wave9090LookAndFeel.cpp
        Source/UI/SkinAssets.cpp
        Source/UI/LabeledControl.cpp
        Source/UI/WavetableScope.cpp
        Source/UI/LedOutputMeter.cpp
)

target_compile_definitions (Wave9090
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries (Wave9090
    PRIVATE
        Wave9090SkinData
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
    Tests/SoundRenderTests.cpp
    Source/Presets/FactoryPresets.cpp
)

target_include_directories (Wave9090Tests PRIVATE Source)
target_compile_features (Wave9090Tests PRIVATE cxx_std_20)

add_test (NAME Wave9090Wavetable COMMAND Wave9090Tests)

add_executable (Wave9090RenderAudit
    Tests/RenderAudit.cpp
    Source/Presets/FactoryPresets.cpp
)
target_include_directories (Wave9090RenderAudit PRIVATE Source)
target_compile_features (Wave9090RenderAudit PRIVATE cxx_std_20)

add_executable (Wave9090PresetDemos
    Tests/PresetDemos.cpp
    Source/Presets/FactoryPresets.cpp
)
target_include_directories (Wave9090PresetDemos PRIVATE Source)
target_compile_features (Wave9090PresetDemos PRIVATE cxx_std_20)

add_executable (Wave9090EliteMeasurements
    Tests/EliteMeasurements.cpp
    Source/Presets/FactoryPresets.cpp
)
target_include_directories (Wave9090EliteMeasurements PRIVATE Source)
target_compile_features (Wave9090EliteMeasurements PRIVATE cxx_std_20)

juce_add_console_app (Wave9090EditorScreenshot PRODUCT_NAME "Wave9090EditorScreenshot")
target_sources (Wave9090EditorScreenshot PRIVATE Tests/EditorScreenshot.cpp)
target_link_libraries (Wave9090EditorScreenshot
    PRIVATE
        Wave9090
        Wave9090SkinData
        juce::juce_audio_utils
        juce::juce_graphics
        juce::juce_gui_basics
)
target_include_directories (Wave9090EditorScreenshot PRIVATE
    Source
    "${CMAKE_BINARY_DIR}/Wave9090/Wave9090_artefacts/JuceLibraryCode"
)
target_compile_definitions (Wave9090EditorScreenshot
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
)
add_test (NAME Wave9090EditorScreenshot COMMAND Wave9090EditorScreenshot)