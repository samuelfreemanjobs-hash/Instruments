cmake_minimum_required(VERSION 3.22)
project(MpcSubPhatty VERSION 1.0.0)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE/CMakeLists.txt")
  message(FATAL_ERROR "JUCE not found. git submodule update --init vst/JUCE")
endif()
add_subdirectory(../JUCE JUCE)

juce_add_plugin(MpcSubPhatty
    COMPANY_NAME "Instruments"
    BUNDLE_ID "com.instruments.mpcsubphatty"
    PLUGIN_MANUFACTURER_CODE Inst
    PLUGIN_CODE SubP
    FORMATS VST3 Standalone
    PRODUCT_NAME "MpcSubPhatty"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE)

target_sources(MpcSubPhatty PRIVATE Source/PluginProcessor.cpp)

target_compile_definitions(MpcSubPhatty
    PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(MpcSubPhatty
    PRIVATE juce::juce_audio_utils juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags
           juce::juce_recommended_warning_flags)