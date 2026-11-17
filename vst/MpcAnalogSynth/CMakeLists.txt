cmake_minimum_required(VERSION 3.22)

project(MpcAnalogSynth VERSION 1.0.0)

option(MPC_VST3_HEADLESS "Skip PluginEditor target sources (Melodee host)" ON)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE/CMakeLists.txt")
  message(FATAL_ERROR "JUCE not found. From repo root: git submodule update --init vst/JUCE")
endif()
add_subdirectory(../JUCE JUCE)

juce_add_plugin(MpcAnalogSynth
    COMPANY_NAME "Instruments"
    BUNDLE_ID "com.instruments.mpcanalogsynth"
    PLUGIN_MANUFACTURER_CODE Inst
    PLUGIN_CODE MpcA
    FORMATS VST3 Standalone
    PRODUCT_NAME "MpcAnalogSynth"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE)

target_sources(MpcAnalogSynth
    PRIVATE
        Source/PluginProcessor.cpp
        Source/MpcSynthVoice.h)

if(NOT MPC_VST3_HEADLESS)
  target_sources(MpcAnalogSynth PRIVATE Source/PluginEditor.cpp)
endif()

target_compile_definitions(MpcAnalogSynth
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(MpcAnalogSynth
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)