cmake_minimum_required(VERSION 3.22)
project(MpcSh101 VERSION 1.0.0)
option(MPC_VST3_HEADLESS "Melodee headless build" ON)
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE/CMakeLists.txt")
  message(FATAL_ERROR "JUCE not found. From repo root: git submodule update --init vst/JUCE")
endif()
add_subdirectory(../JUCE JUCE)
juce_add_plugin(MpcSh101
    COMPANY_NAME "Instruments" BUNDLE_ID com.instruments.mpcsh101
    PLUGIN_MANUFACTURER_CODE Inst PLUGIN_CODE Sh10
    FORMATS VST3 Standalone PRODUCT_NAME "MpcSh101"
    IS_SYNTH TRUE NEEDS_MIDI_INPUT TRUE COPY_PLUGIN_AFTER_BUILD FALSE)
target_sources(MpcSh101 PRIVATE Source/PluginProcessor.cpp Source/FactoryPrograms.h)
target_link_libraries(MpcSh101 PRIVATE juce::juce_audio_utils juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)
target_compile_definitions(MpcSh101
    PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)