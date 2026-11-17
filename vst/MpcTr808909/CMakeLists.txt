cmake_minimum_required(VERSION 3.22)
project(MpcTr808909 VERSION 1.0.0)
option(MPC_VST3_HEADLESS "Melodee headless build" ON)
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE/CMakeLists.txt")
  message(FATAL_ERROR "JUCE not found. From repo root: git submodule update --init vst/JUCE")
endif()
add_subdirectory(../JUCE JUCE)
juce_add_plugin(MpcTr808909
    COMPANY_NAME "Instruments" BUNDLE_ID com.instruments.mpctr808909
    PLUGIN_MANUFACTURER_CODE Inst PLUGIN_CODE Tr8k
    FORMATS VST3 Standalone PRODUCT_NAME "MpcTr808909"
    IS_SYNTH TRUE NEEDS_MIDI_INPUT TRUE COPY_PLUGIN_AFTER_BUILD FALSE)
target_sources(MpcTr808909 PRIVATE Source/PluginProcessor.cpp Source/FactoryPrograms.h)
target_link_libraries(MpcTr808909 PRIVATE juce::juce_audio_utils
    PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)
target_compile_definitions(MpcTr808909
    PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)