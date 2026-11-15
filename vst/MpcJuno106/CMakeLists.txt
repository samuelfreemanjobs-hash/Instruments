cmake_minimum_required(VERSION 3.22)
project(MpcJuno106 VERSION 1.0.0)
option(MPC_VST3_HEADLESS "Melodee headless build" ON)
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE/CMakeLists.txt")
  message(FATAL_ERROR "JUCE not found. git submodule update --init vst/JUCE")
endif()
add_subdirectory(../JUCE JUCE)
juce_add_plugin(MpcJuno106
    COMPANY_NAME "Instruments" BUNDLE_ID com.instruments.mpcjuno106
    PLUGIN_MANUFACTURER_CODE Inst PLUGIN_CODE Jn06
    FORMATS VST3 Standalone PRODUCT_NAME "MpcJuno106"
    IS_SYNTH TRUE NEEDS_MIDI_INPUT TRUE COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_AUTO_MANIFEST FALSE)
target_sources(MpcJuno106 PRIVATE Source/PluginProcessor.cpp Source/JunoVoice.h
    Source/FactoryPrograms.h)
target_link_libraries(MpcJuno106 PRIVATE juce::juce_audio_utils juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_warning_flags)
set_property(TARGET MpcJuno106 PROPERTY INTERPROCEDURAL_OPTIMIZATION FALSE)
target_compile_definitions(MpcJuno106
    PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)