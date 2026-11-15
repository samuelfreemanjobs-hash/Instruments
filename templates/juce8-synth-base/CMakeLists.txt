# Copy into a root-level <PluginName>/CMakeLists.txt after new-vst or manual scaffold.
# Parent must: cmake_minimum_required(3.22), set C++20, add_subdirectory(JUCE).
# See ProphetRev2Trap/CMakeLists.txt and docs/JUCE8_VST3_PROJECT_RULES.md.

cmake_minimum_required(VERSION 3.22)
project(ProphetMod16 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

juce_add_plugin(ProphetMod16
    COMPANY_NAME "Instruments"
    BUNDLE_ID com.instruments.prophetmod16
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Inst
    PLUGIN_CODE Pm16
    FORMATS VST3 Standalone
    PRODUCT_NAME "ProphetMod16")

target_sources(ProphetMod16
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp)

target_compile_definitions(ProphetMod16
    PUBLIC
        JUCE_WEB_BROWSER=1
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(ProphetMod16
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)

target_include_directories(ProphetMod16 PRIVATE Source)

juce_generate_juce_header(ProphetMod16)