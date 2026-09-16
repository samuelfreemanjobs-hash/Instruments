cmake_minimum_required(VERSION 3.22)

project(TemplateEffect VERSION 1.0.0)

set(SHARED_DSP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../shared/dsp)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE/CMakeLists.txt")
  message(FATAL_ERROR "JUCE not found. From repo root: git submodule update --init vst/JUCE")
endif()
add_subdirectory(../JUCE JUCE)

juce_add_plugin(TemplateEffect
    COMPANY_NAME "Instruments"
    BUNDLE_ID "com.instruments.templateeffect"
    PLUGIN_MANUFACTURER_CODE Inst
    PLUGIN_CODE Tefx
    FORMATS VST3 Standalone
    PRODUCT_NAME "Template Effect"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE)

target_sources(TemplateEffect
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp)

target_include_directories(TemplateEffect PRIVATE ${SHARED_DSP_DIR})

target_compile_definitions(TemplateEffect
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(TemplateEffect
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
