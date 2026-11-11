cmake_minimum_required(VERSION 3.22)

project(XDPatchArchitect VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE/CMakeLists.txt")
  message(FATAL_ERROR "JUCE not found. From repo root: git submodule update --init vst/JUCE")
endif()
add_subdirectory(../JUCE JUCE)

juce_add_plugin(XDPatchArchitect
    COMPANY_NAME "Instruments"
    BUNDLE_ID "com.instruments.xdpatcharchitect"
    PLUGIN_MANUFACTURER_CODE Inst
    PLUGIN_CODE XdPa
    FORMATS VST3 Standalone
    PRODUCT_NAME "XD Patch Architect"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE)

target_sources(XDPatchArchitect
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/XdCcMap.cpp)

target_compile_definitions(XDPatchArchitect
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(XDPatchArchitect
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
